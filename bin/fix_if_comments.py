#!/usr/bin/env python3
"""
fix_if_comments.py - C/C++ ファイルの #else / #endif コメントと
Linux/Windows 二択分岐を標準化する。

【規則】
  #ifndef MACRO ... #else /* MACRO */  ... #endif /* MACRO */
  #ifdef  MACRO ... #else /* !MACRO */ ... #endif /* MACRO */

  #if defined(MACRO) ... #endif /* MACRO */
  #if defined(MACRO) ... #else /* !MACRO */ ... #endif /* MACRO */

  #if defined(PLATFORM_LINUX) ... #elif defined(PLATFORM_WINDOWS) ... #endif /* PLATFORM_ */

  - Linux/Windows 二択分岐は #else /* PLATFORM_WINDOWS */ ではなく
    #elif defined(PLATFORM_WINDOWS) に統一する
  - コメントには ifdef/ifndef キーワードは含めない（マクロ名または !マクロ名のみ）
  - 単一マクロの #if defined(MACRO) は #ifdef 相当として #endif にコメントする
  - 単純な #elif defined(MACRO) だけで構成される #if defined() チェーンは、
    共通接頭辞があれば #endif にコメントする
  - 複合条件を含む #elif チェーンの #else / #endif にはコメントしない
  - #if EXPR（複雑な式）のブロックは変更しない

【使い方】
  python3 bin/fix_if_comments.py [--dry-run] <path>...

  --dry-run  ファイルを変更せず、差分のみ表示する
  <path>     ファイルまたはディレクトリ（複数指定可）
             ディレクトリ指定時は .c / .h / .cc / .hpp を再帰的に処理
"""

import sys
import re
import os
import argparse
import difflib
from pathlib import Path


# ---------------------------------------------------------------------------
# パーサー
# ---------------------------------------------------------------------------

_RE_IF = re.compile(r'^(\s*)#\s*(ifdef|ifndef)\s+(\w+)\s*(?:/\*.*\*/)?\s*$')
_RE_IF_DEFINED  = re.compile(r'^(\s*)#\s*if\s+defined\s*\(\s*(\w+)\s*\)\s*(?:/\*.*\*/)?\s*$')
_RE_ELIF_DEFINED = re.compile(r'^(\s*)#\s*elif\s+defined\s*\(\s*(\w+)\s*\)\s*(?:/\*.*\*/)?\s*$')
_RE_ELIF = re.compile(r'^(\s*)#\s*elif\b')
_RE_IF_GENERIC = re.compile(r'^(\s*)#\s*if\b')
_RE_ELSE = re.compile(r'^(\s*)#\s*else\b(.*)')
_RE_ENDIF = re.compile(r'^(\s*)#\s*endif\b(.*)')


def _strip_comment(tail):
    """'  /* FOO */' のような末尾文字列からコメント内容を返す。なければ None。"""
    m = re.match(r'\s*/\*\s*(.*?)\s*\*/\s*$', tail)
    return m.group(1) if m else None


def _common_macro_prefix(macros):
    """複数マクロの共通接頭辞を返す。意味のある接頭辞がない場合は None。"""
    if len(macros) < 2:
        return None

    prefix = macros[0]
    for macro in macros[1:]:
        limit = min(len(prefix), len(macro))
        i = 0
        while i < limit and prefix[i] == macro[i]:
            i += 1
        prefix = prefix[:i]
        if not prefix:
            return None

    boundary = prefix.rfind('_')
    if boundary < 0:
        return None

    prefix = prefix[:boundary + 1]
    return prefix if prefix.strip('_') else None


def analyze(lines):
    """
    ファイルの行リストを解析し、修正が必要な行を特定する。

    Returns:
        fixes: dict { line_index: new_line_string }
    """
    # スタック要素:
    #   ifdef/ifndef : {'kind': 'ifdef'/'ifndef', 'macro': str, 'macros': [], 'has_else': bool, 'has_elif': bool, 'complex_elif': bool}
    #   if defined() : {'kind': 'if_defined', 'macro': '', 'macros': [str, ...], 'has_else': bool, 'has_elif': bool, 'complex_elif': bool}
    #   #if EXPR     : {'kind': 'if', 'macro': '', 'macros': [], 'has_else': bool, 'has_elif': bool, 'complex_elif': bool}
    stack = []
    # else_info[i]  = (kind, macro, macros, has_elif, complex_elif)
    else_info = {}
    # endif_info[i] = (kind, macro, has_else, macros, has_elif, complex_elif)
    endif_info = {}

    for i, line in enumerate(lines):
        # --- #ifdef / #ifndef ---
        m = _RE_IF.match(line)
        if m:
            kind = m.group(2)   # 'ifdef' or 'ifndef'
            macro = m.group(3)
            stack.append({'kind': kind, 'macro': macro, 'macros': [], 'has_else': False, 'has_elif': False, 'complex_elif': False})
            continue

        # --- #if defined(MACRO) ---
        m = _RE_IF_DEFINED.match(line)
        if m:
            macro = m.group(2)
            stack.append({'kind': 'if_defined', 'macro': '', 'macros': [macro], 'has_else': False, 'has_elif': False, 'complex_elif': False})
            continue

        # --- #if EXPR（複雑式、スタックに積むが修正しない） ---
        if _RE_IF_GENERIC.match(line) and not _RE_IF.match(line) and not _RE_IF_DEFINED.match(line):
            stack.append({'kind': 'if', 'macro': '', 'macros': [], 'has_else': False, 'has_elif': False, 'complex_elif': False})
            continue

        # --- #elif defined(MACRO) ---
        m = _RE_ELIF_DEFINED.match(line)
        if m and stack:
            top = stack[-1]
            if top['kind'] == 'if_defined':
                top['has_elif'] = True
                top['macros'].append(m.group(2))
            continue

        # --- #elif EXPR（複合条件を含む #if defined() チェーン） ---
        if _RE_ELIF.match(line) and stack:
            top = stack[-1]
            if top['kind'] == 'if_defined':
                top['has_elif'] = True
                top['complex_elif'] = True
            continue

        # --- #else ---
        m = _RE_ELSE.match(line)
        if m and stack:
            top = stack[-1]
            top['has_else'] = True
            else_info[i] = (top['kind'], top['macro'], top['macros'], top['has_elif'], top['complex_elif'])
            continue

        # --- #endif ---
        m = _RE_ENDIF.match(line)
        if m and stack:
            top = stack.pop()
            endif_info[i] = (top['kind'], top['macro'], top['has_else'], top['macros'], top['has_elif'], top['complex_elif'])
            continue

    # --- 修正箇所の収集 ---
    fixes = {}

    for i, (kind, macro, macros, has_elif, complex_elif) in else_info.items():
        line = lines[i]
        m = _RE_ELSE.match(line)
        indent = m.group(1)
        tail = m.group(2)
        current_comment = _strip_comment(tail)

        if kind in ('ifdef', 'ifndef'):
            expected = macro if kind == 'ifndef' else '!' + macro
            if current_comment != expected:
                fixes[i] = f'{indent}#else /* {expected} */\n'
        elif kind == 'if_defined':
            if complex_elif:
                if current_comment is not None:
                    fixes[i] = f'{indent}#else\n'
                continue

            if macros == ['PLATFORM_LINUX'] and current_comment == 'PLATFORM_WINDOWS':
                fixes[i] = f'{indent}#elif defined(PLATFORM_WINDOWS)\n'
                continue

            expected = ' && '.join('!' + mac for mac in macros) if has_elif else '!' + macros[0]
            if current_comment != expected:
                fixes[i] = f'{indent}#else /* {expected} */\n'

    for i, (kind, macro, has_else, macros, has_elif, complex_elif) in endif_info.items():
        line = lines[i]
        m = _RE_ENDIF.match(line)
        indent = m.group(1)
        tail = m.group(2)
        current_comment = _strip_comment(tail)

        if kind in ('ifdef', 'ifndef'):
            expected = macro  # #endif は常に ! なし
            if current_comment != expected:
                fixes[i] = f'{indent}#endif /* {expected} */\n'
        elif kind == 'if_defined':
            if complex_elif:
                if current_comment is not None:
                    fixes[i] = f'{indent}#endif\n'
                continue

            if has_elif:
                expected = _common_macro_prefix(macros)
                if expected is None:
                    if current_comment is not None:
                        fixes[i] = f'{indent}#endif\n'
                elif current_comment != expected:
                    fixes[i] = f'{indent}#endif /* {expected} */\n'
            else:
                expected = macros[0]
                if current_comment != expected:
                    fixes[i] = f'{indent}#endif /* {expected} */\n'

    return fixes


def process_file(path, dry_run):
    """1 ファイルを処理する。変更があれば diff を表示し、dry_run=False なら書き込む。"""
    try:
        with open(path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except UnicodeDecodeError:
        with open(path, 'r', encoding='latin-1') as f:
            lines = f.readlines()

    fixes = analyze(lines)
    if not fixes:
        return 0

    new_lines = list(lines)
    for i, new_line in fixes.items():
        new_lines[i] = new_line

    rel = os.path.relpath(path)
    diff = list(difflib.unified_diff(
        lines, new_lines,
        fromfile=f'a/{rel}',
        tofile=f'b/{rel}',
    ))
    for d in diff:
        sys.stdout.write(d)

    if not dry_run:
        try:
            with open(path, 'w', encoding='utf-8') as f:
                f.writelines(new_lines)
        except Exception as e:
            print(f'ERROR writing {path}: {e}', file=sys.stderr)
            return 0

    return len(fixes)


def collect_files(paths):
    """パスリストから C/C++ ソース・ヘッダを収集する。"""
    result = []
    suffixes = ('*.c', '*.h', '*.cc', '*.hpp')
    for p in paths:
        p = Path(p)
        if p.is_file():
            result.append(p)
        elif p.is_dir():
            for suffix in suffixes:
                result.extend(sorted(p.rglob(suffix)))
        else:
            print(f'WARNING: {p} は存在しません', file=sys.stderr)
    return result


# ---------------------------------------------------------------------------
# エントリポイント
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description='#else / #endif コメントを標準化する',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument('--dry-run', action='store_true',
                        help='ファイルを変更せず差分のみ表示する')
    parser.add_argument('paths', nargs='+', metavar='PATH',
                        help='処理するファイルまたはディレクトリ')
    args = parser.parse_args()

    files = collect_files(args.paths)
    if not files:
        print('対象ファイルが見つかりません', file=sys.stderr)
        sys.exit(1)

    total_files = 0
    total_fixes = 0

    for f in files:
        n = process_file(f, dry_run=args.dry_run)
        if n:
            total_files += 1
            total_fixes += n

    mode = '(dry-run) ' if args.dry_run else ''
    print(f'\n{mode}{total_files} ファイル / {total_fixes} 箇所 {"修正予定" if args.dry_run else "修正済み"}')


if __name__ == '__main__':
    main()
