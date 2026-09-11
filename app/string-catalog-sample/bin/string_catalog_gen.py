#!/usr/bin/env python3
"""カタログ定義 (JSONC) から string_catalog の生成物 2 ファイルを書き出す。"""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from pathlib import Path

# 引数種別から、型付きラッパーの仮引数の型への対応。
# 正本は prod/include/string_catalog/string_catalog_argument.h の対応表。
ARGUMENT_TYPES = {
    "STRING": "const char *",
    "CHAR": "char",
    "INT8": "int8_t",
    "UINT8": "uint8_t",
    "INT16": "int16_t",
    "UINT16": "uint16_t",
    "INT32": "int32_t",
    "UINT32": "uint32_t",
    "INT64": "int64_t",
    "UINT64": "uint64_t",
    "HEX8": "uint8_t",
    "HEX16": "uint16_t",
    "HEX32": "uint32_t",
    "HEX64": "uint64_t",
    "SIZE": "size_t",
    "SSIZE": "int64_t",
    "POINTER": "const void *",
    "DOUBLE": "double",
    "ERROR_CODE": "int",
}

# 位置指定の添字に書ける最大の桁数。string_catalog_render.c の INDEX_DIGITS_MAX と揃える。
INDEX_DIGITS_MAX = 2

# 1 つの文字列が取れる引数の最大個数。STRING_CATALOG_ARGUMENT_MAX と揃える。
ARGUMENT_MAX = 32


class DefinitionError(Exception):
    """カタログ定義の内容が不正であることを表す。"""


def strip_jsonc(text: str) -> str:
    """JSONC から行コメント、ブロック コメント、末尾コンマを取り除く。

    文字列リテラルの中は書き換えない。エスケープも解釈する。
    """
    out = []
    index = 0
    length = len(text)
    in_string = False

    while index < length:
        char = text[index]

        if in_string:
            out.append(char)
            if char == "\\" and (index + 1) < length:
                out.append(text[index + 1])
                index += 2
                continue
            if char == '"':
                in_string = False
            index += 1
            continue

        if char == '"':
            in_string = True
            out.append(char)
            index += 1
            continue

        if text.startswith("//", index):
            while index < length and text[index] != "\n":
                index += 1
            continue

        if text.startswith("/*", index):
            end = text.find("*/", index + 2)
            index = length if end < 0 else end + 2
            continue

        if char == ",":
            # 末尾コンマなら落とす。次の非空白が閉じ括弧のとき。
            probe = index + 1
            while probe < length and text[probe] in " \t\r\n":
                probe += 1
            if probe < length and text[probe] in "}]":
                index += 1
                continue

        out.append(char)
        index += 1

    return "".join(out)


def load_definition(path: Path) -> dict:
    """定義ファイルを読み込む。"""
    text = path.read_text(encoding="utf-8")
    try:
        return json.loads(strip_jsonc(text))
    except json.JSONDecodeError as error:
        raise DefinitionError(f"{path}: JSON として解釈できません: {error}") from error


def join_text(value) -> str:
    """文字列、または文字列の配列を 1 つの文字列にする。"""
    if isinstance(value, str):
        return value
    if isinstance(value, list):
        if not all(isinstance(item, str) for item in value):
            raise DefinitionError("文字列の配列に文字列以外が含まれています。")
        return " ".join(value)
    raise DefinitionError(f"文字列または文字列の配列である必要があります: {value!r}")


def placeholder_indices(text: str) -> list[int]:
    """書式に現れる位置指定の添字を返す。構文が不正なら例外にする。

    受け付ける構文は string_catalog_render.c の render_scan_text と同じ。
    """
    found = []
    index = 0
    length = len(text)

    while index < length:
        char = text[index]

        if char == "}":
            if not text.startswith("}}", index):
                raise DefinitionError(f"対を成さない }} があります: {text!r}")
            index += 2
            continue

        if char != "{":
            index += 1
            continue

        if text.startswith("{{", index):
            index += 2
            continue

        digits = 0
        while (
            digits < INDEX_DIGITS_MAX
            and (index + 1 + digits) < length
            and text[index + 1 + digits].isdigit()
            and text[index + 1 + digits].isascii()
        ):
            digits += 1

        if digits == 0 or (index + 1 + digits) >= length or text[index + 1 + digits] != "}":
            raise DefinitionError(f"位置指定の構文が不正です: {text!r}")
        if digits > 1 and text[index + 1] == "0":
            raise DefinitionError(f"位置指定の添字に先頭のゼロは書けません: {text!r}")

        found.append(int(text[index + 1 : index + 1 + digits]))
        index += 2 + digits

    return found


def validate(document: dict) -> list[dict]:
    """定義の内容を検査し、文字列の一覧を返す。"""
    for key in ("library_prefix", "module_prefix", "id_enum", "languages", "strings"):
        if key not in document:
            raise DefinitionError(f"必須の項目がありません: {key}")

    languages = document["languages"]
    if "neutral" not in languages:
        raise DefinitionError("languages に neutral が必要です。")

    strings = document["strings"]
    if not strings:
        raise DefinitionError("strings が空です。")

    seen_ids: set[str] = set()
    seen_id_texts: set[str] = set()

    for entry in strings:
        for key in ("id", "id_text", "category", "brief", "summary", "arguments", "texts", "notes"):
            if key not in entry:
                raise DefinitionError(f"{entry.get('id', '?')}: 必須の項目がありません: {key}")

        # 分類値は生値とする。生成物を特定の app の列挙から独立させるため。
        if not isinstance(entry["category"], int) or isinstance(entry["category"], bool):
            raise DefinitionError(f"{entry['id']}: category は整数で指定してください。")

        if entry["id"] in seen_ids:
            raise DefinitionError(f"文字列 ID が重複しています: {entry['id']}")
        seen_ids.add(entry["id"])

        if entry["id_text"] in seen_id_texts:
            raise DefinitionError(f"固定文字列が重複しています: {entry['id_text']}")
        seen_id_texts.add(entry["id_text"])

        arguments = entry["arguments"]
        if len(arguments) > ARGUMENT_MAX:
            raise DefinitionError(f"{entry['id']}: 引数が上限 {ARGUMENT_MAX} 個を超えています。")

        for argument in arguments:
            for key in ("kind", "name", "description"):
                if key not in argument:
                    raise DefinitionError(f"{entry['id']}: 引数に {key} がありません。")
            if argument["kind"] not in ARGUMENT_TYPES:
                raise DefinitionError(f"{entry['id']}: 未知の引数種別です: {argument['kind']}")

        for section in ("texts", "notes"):
            if "neutral" not in entry[section]:
                raise DefinitionError(f"{entry['id']}: {section} に neutral が必要です。")
            for language in entry[section]:
                if language not in languages:
                    raise DefinitionError(f"{entry['id']}: languages にない言語です: {language}")

        for language, text in entry["texts"].items():
            indices = placeholder_indices(join_text(text))
            for found in indices:
                if found >= len(arguments):
                    raise DefinitionError(
                        f"{entry['id']}: {language} の位置指定 {{{found}}} が引数個数 {len(arguments)} を超えています。"
                    )

    return strings


GENERATED_NOTE = """ *  本ヘッダーと `{source}` は、カタログ定義 `{definition}` からの生成物です。\\n
 *  列挙と表は 1 組の生成単位であり、常に同時に生成してください。\\n
 *  手作業で編集せず、生成元の定義を変更してから `bin/string_catalog_gen.py` を実行してください。"""


def c_string(text: str) -> str:
    """C の文字列リテラルへ変換する。"""
    escaped = text.replace("\\", "\\\\").replace('"', '\\"')
    return f'"{escaped}"'


def language_constant(document: dict, language: str) -> str:
    """言語のキーを、ライブラリの列挙定数へ変換する。"""
    return f"{document['library_prefix'].upper()}_LANGUAGE_{language.upper()}"


def kind_constant(document: dict, kind: str) -> str:
    """引数種別のキーを、ライブラリの列挙定数へ変換する。"""
    return f"{document['library_prefix'].upper()}_ARGUMENT_KIND_{kind}"


def wrapper_name(document: dict, string_id: str) -> str:
    """文字列 ID から型付きラッパーの関数名を導出する。"""
    return f"{document['module_prefix']}_{string_id.lower()}"


def doc_lines(text: str, indent: str, width: int = 112) -> list[str]:
    """Doxygen 本文の 1 段落を、指定幅で折り返した行にする。"""
    words = text.split()
    lines: list[str] = []
    current = indent

    for word in words:
        candidate = f"{current}{word}" if current == indent else f"{current} {word}"
        if current != indent and len(candidate) > width:
            lines.append(current)
            current = f"{indent}{word}"
        else:
            current = candidate

    if current != indent:
        lines.append(current)
    return lines


def emit_wrapper(document: dict, entry: dict) -> str:
    """1 件分の型付きラッパーを、Doxygen コメントとともに書き出す。"""
    name = wrapper_name(document, entry["id"])
    arguments = entry["arguments"]

    names = ["dest", "dest_size"] + [argument["name"] for argument in arguments]
    name_width = max(len(name) for name in names) + 1
    # `     *  ` の 8 文字と、`@param[out]     ` の 16 文字のあとに名前欄が並ぶ
    continuation = "     *" + " " * (8 + 16 + name_width - 6)

    lines = ["    /**"]
    lines.append(f"     *  @brief          {entry['summary']}")
    lines.append(f"     *  @param[out]     {'dest'.ljust(name_width)}文字列の格納先。NULL を渡してはなりません。")
    lines.append(
        f"     *  @param[in]      {'dest_size'.ljust(name_width)}@p dest のバイト数。1 以上を指定してください。"
    )

    for argument in arguments:
        padded = argument["name"].ljust(name_width)
        lines.append(f"     *  @param[in]      {padded}{argument['description']}")
        lines.append(f"{continuation}引数種別は @ref {kind_constant(document, argument['kind'])} です。")

    lines.append(
        f"     *  @return         戻り値は @ref {document['library_prefix']}_format と同じです。"
    )

    if not arguments:
        lines.append("     *")
        lines.append("     *  この文字列は引数を取りません。")

    if entry.get("remarks"):
        lines.append("     *")
        lines.extend(doc_lines(join_text(entry["remarks"]), "     *  "))

    lines.append("     *")
    lines.append("     *  @par            書式")
    texts = entry["texts"]
    languages = [language for language in document["languages"] if language in texts]
    for position, language in enumerate(languages):
        suffix = "\\n" if position < (len(languages) - 1) else ""
        lines.append(f"     *  `{join_text(texts[language])}`{suffix}")
    lines.append("     */")

    parameters = ["char *dest", "const size_t dest_size"]
    for argument in arguments:
        c_type = ARGUMENT_TYPES[argument["kind"]]
        if c_type.endswith("*"):
            parameters.append(f"{c_type}{argument['name']}")
        else:
            parameters.append(f"const {c_type} {argument['name']}")

    call = [f"{document['module_prefix']}_catalog()", "dest", "dest_size", entry["id"]]
    call.extend(argument["name"] for argument in arguments)

    lines.append(f"    static inline int {name}({', '.join(parameters)})")
    lines.append("    {")
    lines.append(f"        return {document['library_prefix']}_format({', '.join(call)});")
    lines.append("    }")

    return "\n".join(lines)


def expand(template: str, module: str, library: str) -> str:
    """テンプレート中の接頭辞の目印を置き換える。

    テンプレートは C のコードを含み波括弧が現れるため、str.format は使わない。
    """
    return template.replace("@MODULE@", module).replace("@LIBRARY@", library)


def emit_header(document: dict, strings: list[dict], definition_name: str) -> str:
    """ヘッダー側の生成物を組み立てる。"""
    module = document["module_prefix"]
    library = document["library_prefix"]
    guard = f"{module.upper()}_H"
    header_name = f"{module}.h"
    source_name = f"{module}.c"
    module_dir = document.get("module_dir", ".")

    out = [
        "/**",
        " " + "*" * 79,
        f" *  @file           {header_name}",
        f" *  @brief          利用者が定義する文字列 ID の列挙と、カタログの取得を宣言します。",
        f" *  @author         {document.get('author', '')}",
        f" *  @date           {document.get('date', '')}",
        f" *  @version        {document.get('version', '')}",
        " *",
        f" *  本ヘッダーは `{module_dir}/` のモジュール私有ヘッダーです。\\n",
        f' *  同ディレクトリの実装ファイルからだけ `#include "{header_name}"` で取り込みます。',
        " *",
        GENERATED_NOTE.format(source=source_name, definition=definition_name),
        " *",
        " *  この 2 ファイルが、文字列カタログを利用するアプリケーションが用意する部分です。\\n",
        " *  言語、引数種別、書式の構文はライブラリが定めます。\\n",
        " *  分類値の意味付けは利用者の取り決めであり、別ヘッダーで手書きします。",
        " *",
        " *  列挙の名前をライブラリの接頭辞に揃えているのは、生成物の識別子がカタログ定義に由来するためです。\\n",
        f" *  ライブラリはこの名前を定義せず、文字列 ID を `int` として受け取ります。",
        " *",
        f" *  @copyright      Copyright (C) {document.get('author', '')}. 2026. All rights reserved.",
        " *",
        " " + "*" * 79,
        " */",
        "",
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
    ]

    out.extend(
        [
            f"#include <{library}/{library}_spec.h>",
            "#include <stdarg.h>",
            "#include <stddef.h>",
            "#include <stdint.h>",
            "",
            "#ifdef __cplusplus",
            'extern "C"',
            "{",
            "#endif /* __cplusplus */",
            "",
            "    /**",
            "     *  @brief          カタログに登録した文字列を識別します。",
            "     *",
            "     *  各 ID の引数スキーマ、分類値、言語別の書式と備考は、同じ生成単位の表が保持します。\\n",
            "     *  値は生成のたびに並び順から決まります。ログの解析で安定して使う識別子は固定文字列です。",
            "     */",
            f"    typedef enum {document['id_enum']}",
            "    {",
        ]
    )

    for position, entry in enumerate(strings):
        comma = "," if position < (len(strings) - 1) else ""
        out.append(f"        {entry['id']} = {position + 1}{comma} /**< {entry['brief']} */")

    out.append(f"    }} {document['id_enum']};")
    out.append("")
    out.append(expand(ACCESSOR_DECLARATIONS, module, library))

    for entry in strings:
        out.append(emit_wrapper(document, entry))
        out.append("")

    out.extend(
        [
            "#ifdef __cplusplus",
            "}",
            "#endif /* __cplusplus */",
            "",
            f"#endif /* {guard} */",
            "",
        ]
    )

    return "\n".join(out)


ACCESSOR_DECLARATIONS = """\
    /**
     *  @brief          カタログの先頭を返します。
     *  @return         カタログの配列です。NULL は返しません。
     *
     *  返すポインターは静的領域を指します。呼び出し側で解放してはなりません。\\n
     *  @ref @MODULE@_entry_count とともに @ref @LIBRARY@ を組み立てる材料です。\\n
     *  組み立て済みのカタログは @ref @MODULE@_catalog が返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    const @LIBRARY@_entry *@MODULE@_entries(void);

    /**
     *  @brief          カタログの件数を返します。
     *  @return         文字列の件数です。1 以上を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    int @MODULE@_entry_count(void);

    /**
     *  @brief          文字列 ID からカタログの添字を引く表を返します。
     *  @return         添字表です。NULL は返しません。
     *
     *  文字列 ID を添字として、カタログの添字を格納します。\\n
     *  登録していない添字には負の値を格納します。\\n
     *  この表により、文字列 ID からカタログを引く探索を線形探索から添字引きへ置き換えます。
     *
     *  返すポインターは静的領域を指します。呼び出し側で解放してはなりません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    const int *@MODULE@_id_index(void);

    /**
     *  @brief          添字表の要素数を返します。
     *  @return         添字表の要素数です。最大の文字列 ID に 1 を加えた値です。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    int @MODULE@_id_index_count(void);

    /**
     *  @brief          このカタログ定義のカタログ識別オブジェクトを返します。
     *  @return         カタログ識別オブジェクトです。NULL は返しません。
     *
     *  配列と添字表を 1 つのカタログへまとめた値です。\\n
     *  ライブラリはカタログを保持しないため、組み立て API へはこの値を渡します。
     *
     *  返すポインターは静的領域を指します。呼び出し側で解放してはなりません。\\n
     *  ほかのカタログ定義と組み合わせる場合は、それぞれのカタログを使い分けます。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    const @LIBRARY@ *@MODULE@_catalog(void);

    /**
     *  @brief          このカタログ定義を使用して、文字列を組み立てます。
     *  @param[out]     dest      文字列の格納先。NULL を渡してはなりません。
     *  @param[in]      dest_size @p dest のバイト数。1 以上を指定してください。
     *  @param[in]      string_id 組み立てる文字列の ID。
     *  @param[in]      ...       引数スキーマが定める順序と型の値。
     *  @return         戻り値は @ref @LIBRARY@_format と同じです。
     *
     *  カタログを省略して呼び出す口です。\\n
     *  @ref @MODULE@_catalog を補って @ref @LIBRARY@_format を呼び出します。
     *
     *  @par            スレッド セーフ
     *  スレッド セーフ性は @ref @LIBRARY@_format と同じです。
     */
    int @MODULE@_format(char *dest, size_t dest_size, int string_id, ...);

    /**
     *  @brief          このカタログ定義を使用して、@c va_list から文字列を組み立てます。
     *  @param[out]     dest      文字列の格納先。NULL を渡してはなりません。
     *  @param[in]      dest_size @p dest のバイト数。1 以上を指定してください。
     *  @param[in]      string_id 組み立てる文字列の ID。
     *  @param[in]      args      引数スキーマが定める順序と型の値を保持する引数リスト。
     *  @return         戻り値は @ref @LIBRARY@_vformat と同じです。
     *
     *  @par            スレッド セーフ
     *  スレッド セーフ性は @ref @LIBRARY@_vformat と同じです。
     */
    int @MODULE@_vformat(char *dest, size_t dest_size, int string_id, va_list args);

    /**
     *  @brief          このカタログ定義の内容を確認します。
     *  @param[out]     string_id_out 不正を検出した文字列の ID。不要な場合は NULL を指定できます。
     *  @param[out]     language_out  不正を検出した言語。不要な場合は NULL を指定できます。
     *  @return         戻り値は @ref @LIBRARY@_verify と同じです。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int @MODULE@_verify(int *string_id_out, @LIBRARY@_language *language_out);

    /**
     *  @brief          このカタログ定義から、文字列の分類値を返します。
     *  @param[in]      string_id 参照する文字列の ID。
     *  @return         戻り値は @ref @LIBRARY@_category と同じです。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int @MODULE@_category(int string_id);

    /**
     *  @brief          このカタログ定義から、文字列 ID の固定文字列を返します。
     *  @param[in]      string_id 参照する文字列の ID。
     *  @return         戻り値は @ref @LIBRARY@_id_text と同じです。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    const char *@MODULE@_id_text(int string_id);

    /**
     *  @brief          このカタログ定義から、現在の言語で文字列の備考を返します。
     *  @param[in]      string_id 参照する文字列の ID。
     *  @return         戻り値は @ref @LIBRARY@_note と同じです。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    const char *@MODULE@_note(int string_id);

    /*
     *  ここから下は、文字列 ID ごとに引数の型を固定したラッパーです。
     *
     *  可変長引数の口はコンパイラが引数の個数と型を検査できません。
     *  書式と引数スキーマがカタログの中にあり、書式文字列が呼び出しの実引数ではないためです。
     *  型付きのラッパーを通すと、通常のプロトタイプ検査によって個数と型の誤りがビルド時に止まります。
     *
     *  実体を持つ翻訳単位を増やさないよう、`static inline` 関数として提供します。
     *  文字列 ID の数だけ公開シンボルが増えることを避けます。
     *
     *  関数名は文字列 ID から機械的に導出します。
     *  文字列 ID の定数名をそのまま小文字化し、`@MODULE@_` を前置します。
     *  接頭辞の除去や語の入れ替えを行わないため、規則に例外がありません。
     *  導出規則の全体は docs/architecture.md を参照してください。
     */
"""


SOURCE_TAIL = """\
/* Doxygen コメントは、ヘッダーに記載 */

const @LIBRARY@_entry *@MODULE@_entries(void)
{
    return s_entries;
}

/* Doxygen コメントは、ヘッダーに記載 */

int @MODULE@_entry_count(void)
{
    return ENTRY_COUNT;
}

/* Doxygen コメントは、ヘッダーに記載 */

const int *@MODULE@_id_index(void)
{
    return s_id_index;
}

/* Doxygen コメントは、ヘッダーに記載 */

int @MODULE@_id_index_count(void)
{
    return ID_INDEX_COUNT;
}

/**
 *  @brief          このカタログ定義のカタログ識別オブジェクトです。
 *
 *  配列と添字表を 1 つのカタログへまとめます。\\n
 *  すべてのメンバーを初期化子で与えられるため `const` とし、初期化関数を持ちません。
 */
static const @LIBRARY@ s_catalog = {s_entries, s_id_index, ENTRY_COUNT, ID_INDEX_COUNT};

/* Doxygen コメントは、ヘッダーに記載 */

const @LIBRARY@ *@MODULE@_catalog(void)
{
    return &s_catalog;
}

/* Doxygen コメントは、ヘッダーに記載 */

int @MODULE@_vformat(char *dest, const size_t dest_size, const int string_id, va_list args)
{
    return @LIBRARY@_vformat(&s_catalog, dest, dest_size, string_id, args);
}

/* Doxygen コメントは、ヘッダーに記載 */

int @MODULE@_format(char *dest, const size_t dest_size, const int string_id, ...)
{
    va_list args;
    int ret;

    va_start(args, string_id);
    ret = @LIBRARY@_vformat(&s_catalog, dest, dest_size, string_id, args);
    va_end(args);

    return ret;
}

/* Doxygen コメントは、ヘッダーに記載 */

int @MODULE@_verify(int *string_id_out, @LIBRARY@_language *language_out)
{
    return @LIBRARY@_verify(&s_catalog, string_id_out, language_out);
}

/* Doxygen コメントは、ヘッダーに記載 */

int @MODULE@_category(const int string_id)
{
    return @LIBRARY@_category(&s_catalog, string_id);
}

/* Doxygen コメントは、ヘッダーに記載 */

const char *@MODULE@_id_text(const int string_id)
{
    return @LIBRARY@_id_text(&s_catalog, string_id);
}

/* Doxygen コメントは、ヘッダーに記載 */

const char *@MODULE@_note(const int string_id)
{
    return @LIBRARY@_note(&s_catalog, string_id);
}
"""


def emit_source(document: dict, strings: list[dict], definition_name: str) -> str:
    """実装側の生成物を組み立てる。"""
    module = document["module_prefix"]
    library = document["library_prefix"]
    header_name = f"{module}.h"
    source_name = f"{module}.c"
    # @file はリポジトリの慣習に合わせ、prod/ を除いた相対パスで示す
    module_dir = document.get("module_dir", ".")
    source_display = module_dir[len("prod/") :] if module_dir.startswith("prod/") else module_dir
    last_id = strings[-1]["id"]

    out = [
        "/**",
        " " + "*" * 79,
        f" *  @file           {source_display}/{source_name}",
        " *  @brief          文字列 ID ごとの引数スキーマ、分類値、メタデータ、言語別リソースを保持します。",
        f" *  @author         {document.get('author', '')}",
        f" *  @date           {document.get('date', '')}",
        f" *  @version        {document.get('version', '')}",
        " *",
        f" *  本ファイルは、カタログ定義 `{definition_name}` からの生成物です。\\n",
        f" *  同じ生成元から作る `{header_name}` と合わせて 1 組の生成単位です。\\n",
        " *  手作業で編集せず、生成元の定義を変更してから `bin/string_catalog_gen.py` を実行してください。",
        " *",
        " *  この表は利用者が用意する部分であり、ライブラリは抱え込みません。\\n",
        " *  配列と添字表を @ref s_catalog へまとめ、組み立て API の呼び出しごとに渡します。\\n",
        " *  カタログを省略して呼び出す口も、この生成物が用意します。",
        " *",
        " *  分類値はライブラリが解釈しない補足情報です。\\n",
        " *  意味と有効な範囲は利用者が決めます。生成物は生値のまま保持し、特定の列挙へ依存しません。",
        " *",
        " *  カタログの配列に加えて、文字列 ID を添字とする添字表を持ちます。\\n",
        " *  ライブラリはこの表によって文字列 ID からカタログを直接引き、線形探索を避けます。",
        " *",
        " *  各要素は、文字列 ID、分類値、引数個数、明示的アラインメント、引数スキーマ、",
        " *  文字列 ID の固定文字列、言語別の書式、言語別の備考の順です。\\n",
        f" *  `texts` と `notes` は、@ref {library}_language をキーとした指示付き初期化子で記載します。\\n",
        " *  記載しなかった言語の要素は暗黙にヌル ポインターとなり、ニュートラル言語の要素へ読み替えます。",
        " *",
        " *  引数の型と文字列表現はこの表が決め、言語別リソースは語順だけを決めます。\\n",
        " *  書式中の `{0}` から `{31}` は引数の位置を表します。\\n",
        " *  `{` と `}` そのものを出力する場合は `{{` と `}}` を使用します。",
        " *",
        " *  ニュートラル言語の書式は、英語と同じ表現とします。\\n",
        " *  英語の要素は記載せず、ニュートラル言語の書式へ読み替えます。\\n",
        " *  英語をニュートラル言語と分ける必要が生じた時点で、英語の要素を追加してください。",
        " *",
        " *  ソース ファイルの文字コードは UTF-8 です。出力する文字列も UTF-8 です。",
        " *",
        f" *  @copyright      Copyright (C) {document.get('author', '')}. 2026. All rights reserved.",
        " *",
        " " + "*" * 79,
        " */",
        "",
        f'#include "{header_name}"',
        "",
        "#include <assert.h>",
        "#include <stdarg.h>",
        "#include <stddef.h>",
        "",
        "/** 文字列 ID ごとのカタログです。文字列 ID の昇順に並べます。 */",
        f"static const {library}_entry s_entries[] = {{",
    ]

    rows = []
    for entry in strings:
        arguments = entry["arguments"]
        if arguments:
            kinds = ", ".join(kind_constant(document, argument["kind"]) for argument in arguments)
            kinds_line = f"     {{{kinds}}},"
        else:
            kinds_line = "     {0}, /* 引数なし */"

        row = [
            f"    {{{entry['id']},",
            f"     {entry['category']},",
            f"     {len(arguments)},",
            "     0, /* 明示的アラインメント */",
            kinds_line,
            f"     {c_string(entry['id_text'])},",
        ]

        for section in ("texts", "notes"):
            items = []
            for language in document["languages"]:
                if language in entry[section]:
                    constant = language_constant(document, language)
                    items.append(f"[{constant}] = {c_string(join_text(entry[section][language]))}")
            separator = ",\n      "
            terminator = "}," if section == "texts" else "}}"
            row.append(f"     {{{separator.join(items)}{terminator}")

        rows.append("\n".join(row))

    out.append(",\n".join(rows) + "};")
    out.extend(
        [
            "",
            "/** @ref s_entries の要素数です。 */",
            "#define ENTRY_COUNT ((int)(sizeof(s_entries) / sizeof(s_entries[0])))",
            "",
            "/** 添字表で、文字列 ID を登録していないことを表す値です。 */",
            "#define ID_INDEX_ABSENT (-1)",
            "",
            "/**",
            " *  @brief          文字列 ID を添字として、@ref s_entries の添字を引く表です。",
            " *",
            " *  文字列 ID は 1 から始まるため、添字 0 は使用しません。\\n",
            " *  文字列 ID を歯抜けにする場合は、該当する添字へ @ref ID_INDEX_ABSENT を格納します。",
            " */",
            "static const int s_id_index[] = {",
            "    ID_INDEX_ABSENT, /* 0: 未使用 */",
        ]
    )

    for position, entry in enumerate(strings):
        comma = "," if position < (len(strings) - 1) else ""
        out.append(f"    {position}{comma} /* {entry['id']} */")

    out.extend(
        [
            "};",
            "",
            "/** @ref s_id_index の要素数です。 */",
            "#define ID_INDEX_COUNT ((int)(sizeof(s_id_index) / sizeof(s_id_index[0])))",
            "",
            "/*",
            " *  添字表が最大の文字列 ID を覆っていることを、ビルド時に確かめます。",
            " *  覆っていない文字列 ID は線形探索へ落ちるため動作はしますが、添字表の拡張漏れです。",
            " *  対象は文字列 ID の昇順で最後の定数です。",
            " */",
            f'static_assert(ID_INDEX_COUNT > {last_id}, "id_index must cover every string id");',
            "",
            expand(SOURCE_TAIL, module, library).rstrip("\n"),
            "",
        ]
    )

    return "\n".join(out)


def find_clang_format_style(start: Path) -> Path | None:
    """出力先から親をたどって .clang-format を探す。"""
    for directory in [start.resolve()] + list(start.resolve().parents):
        candidate = directory / ".clang-format"
        if candidate.is_file():
            return candidate
    return None


def format_source(text: str, filename: str, style: Path | None) -> str:
    """生成した内容を clang-format へ通す。

    生成物はリポジトリの整形規則に従う必要があり、整形まで生成器の責務とする。
    そうしないと --check が常に差分を報告することになる。
    """
    if style is None or shutil.which("clang-format") is None:
        print("警告: clang-format が見つからないため、整形せずに出力します。", file=sys.stderr)
        return text

    completed = subprocess.run(
        ["clang-format", f"--style=file:{style}", f"--assume-filename={filename}"],
        input=text,
        capture_output=True,
        text=True,
        check=True,
    )
    return completed.stdout


def main(argv: list[str] | None = None) -> int:
    """コマンドの入口。"""
    parser = argparse.ArgumentParser(description="カタログ定義から string_catalog の生成物を書き出します。")
    parser.add_argument("definition", type=Path, help="カタログ定義 (JSONC) のパス")
    parser.add_argument("--out-dir", type=Path, default=None, help="出力先。既定は定義ファイルと同じ場所")
    parser.add_argument("--check", action="store_true", help="書き出さず、既存の生成物と一致するかだけ確かめる")
    parser.add_argument(
        "--if-newer",
        action="store_true",
        help="生成物が定義ファイルと生成器より新しければ何もしない。make の parse 時に呼ぶ用",
    )
    args = parser.parse_args(argv)

    try:
        document = load_definition(args.definition)
        strings = validate(document)
    except DefinitionError as error:
        print(f"エラー: {error}", file=sys.stderr)
        return 1

    out_dir = args.out_dir if args.out_dir is not None else args.definition.parent
    out_dir.mkdir(parents=True, exist_ok=True)
    definition_name = args.definition.name
    module = document["module_prefix"]

    if args.if_newer and not args.check:
        targets = [out_dir / f"{module}.h", out_dir / f"{module}.c"]
        sources = [args.definition, Path(__file__)]
        if all(target.exists() for target in targets):
            newest_source = max(source.stat().st_mtime for source in sources)
            oldest_target = min(target.stat().st_mtime for target in targets)
            if oldest_target >= newest_source:
                return 0

    style = find_clang_format_style(out_dir)
    outputs = {
        out_dir / f"{module}.h": format_source(
            emit_header(document, strings, definition_name), f"{module}.h", style
        ),
        out_dir / f"{module}.c": format_source(
            emit_source(document, strings, definition_name), f"{module}.c", style
        ),
    }

    differs = False
    for path, content in outputs.items():
        if args.check:
            current = path.read_text(encoding="utf-8") if path.exists() else ""
            if current != content:
                print(f"差分あり: {path}", file=sys.stderr)
                differs = True
        else:
            path.write_text(content, encoding="utf-8", newline="\n")
            print(f"生成: {path}")

    return 1 if differs else 0


if __name__ == "__main__":
    sys.exit(main())
