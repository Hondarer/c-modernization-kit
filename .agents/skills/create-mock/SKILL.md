---
name: create-mock
description: |
  app 配下の C ライブラリ関数 mock を作成するときに使うスキルです。
  include_override による差し替え、Mock クラスへの追加、
  ON_CALL の既定値、app 向け mock 実装の方針をまとめます。
when_to_use: |
  - app/<name>/test 配下で新しい mock 関数を追加するとき
  - app 向け mock の配置先や実装方針を確認したいとき
---

# app 向け C mock 作成

このスキルは `app/<name>/test/` 配下の mock を対象にします。
共通ルールは `framework/testfw/docs/how-to-mock.md` を参照し、この文書では app 固有の判断基準と実装形を扱います。

## mock の目的

prod の各関数に対応する mock を test 側に作成し、単体テストが対象ファイル 1 本だけをコンパイル・リンクして動作を検証できるようにします。
依存先の関数を mock で差し替えることで、テスト対象のソースファイル 1 本のカバレッジを他の実装に依存せず充足します。

mock の実装は `app/<name>/test/libsrc/mock_<lib>/` に配置します。

## app mock と testfw mock の区別

app 向け mock はアプリケーション層の関数（`calcHandler`, `add` 等）を対象にします。
`printf`, `fopen`, `malloc` などの標準ライブラリ関数のモックは testfw 配下に整備されており、app mock とは明確に異なります。

`mock_com_util` はこのスキルの一般ルールの例外です。
`mock_com_util` では全関数について、未注入時および mock 注入時の既定動作を実ライブラリ呼び出しとし、
テストコード側で `EXPECT_CALL` / `ON_CALL` を上書きして `com_util` 単体および `com_util` 利用モジュールの単体テストを行います。
`app/com_util/test/` 配下を扱うときは `.agents/skills/create-mock-com-mock/SKILL.md` を優先します。

標準ライブラリのモックが不足している場合は `create-testfw-mock` スキルを使い、`framework/testfw/libsrc/` 配下にモックを追加します。

## 方針

- app 向け mock は完全 mock 方針です。mock 未注入時は本物へ委譲せず、関数ごとの失敗値を返します。
- `delegate_real_` と `delegate_fake_` は使いません。
- 関数本体には `WEAK_ATR` を付けます。
- シグネチャに `file`, `line`, `func` は追加しません。
- `ON_CALL` の既定値は、テストで明示していない呼び出しが成功扱いにならない値を設定します。

## 変更対象

- `app/<name>/test/include/<lib>/<module>/mock_<module>.h`
  - mock 関数宣言と置換マクロを定義します。
- `app/<name>/test/include_override/<lib>/<module>/<module>.h`
  - 本物のヘッダーを読み込んだ後に置換マクロを有効化します。
- `app/<name>/test/include/mock_<lib>.h`
  - Mock クラスへ `MOCK_METHOD` を追加します。
- `app/<name>/test/libsrc/mock_<lib>/mock_<lib>.cc`
  - コンストラクターへ `ON_CALL` の既定値を追加します。
- `app/<name>/test/libsrc/mock_<lib>/<module>/mock_<func>.cc`
  - 関数本体を実装します。

`com_util` の参照実装は `app/com_util/test/libsrc/mock_com_util/crt/` を基準にします。
このワークツリーでは `app/com_util/test/include/mock_com_util.h` が実在する参照先です。
ただし `mock_com_util` 自体の既定方針はこの節の一般ルールではなく、
`create-mock-com-mock` に記載した real delegate 方針を適用します。

## mock ヘッダー

override ヘッダー経由でだけ置換マクロが有効になる形にします。
app 向けでは delegate 宣言を置きません。

このワークツリーには app 側の代表的な mock ヘッダー実体が揃っていないため、app 文書では固定の雛形コードより次の要件を優先します。

- 置換マクロは `_IN_OVERRIDE_HEADER_...` のようなフラグで局所的に有効にします。
- 関数宣言名と置換先の関係は、対象ライブラリの既存パターンに合わせます。
- `com_util` の `crt` 参照実装では、関数本体は公開関数名そのものに `WEAK_ATR` を付ける形です。
- testfw 向けの `delegate_real_` 宣言や `file`, `line`, `func` 付きシグネチャは持ち込みません。

## Mock クラス

`app/<name>/test/include/mock_<lib>.h` に `MOCK_METHOD` を追加します。

```cpp
MOCK_METHOD(int, com_util_access, (const char *, int));
MOCK_METHOD(FILE *, com_util_fopen, (const char *, const char *, int *));
MOCK_METHOD(int, com_util_sscanf, (const char *, const char *, va_list));
```

- 引数なし関数は `()` を使います。
- 可変長引数そのものは `MOCK_METHOD` に書かず、`va_list` を受け取る形へ変換します。

参照先:
- `app/com_util/test/include/mock_com_util.h`

## ON_CALL の既定値

app 向けでは mock が注入されていても `EXPECT_CALL` や `WillOnce` が書かれていない場合に失敗値へ落ちるようにします。

```cpp
ON_CALL(*this, com_util_access(_, _))
    .WillByDefault(Return(-1));

ON_CALL(*this, com_util_fopen(_, _, _))
    .WillByDefault(Return(nullptr));

ON_CALL(*this, com_util_sscanf(_, _, _))
    .WillByDefault(Return(0));
```

代表的な既定値の目安:

- `int` の成否関数: `-1`
- ポインター戻り値: `nullptr`
- `sscanf` / `vsscanf`: `0`
- `void`: `Return()`

参照先:
- `app/com_util/test/libsrc/mock_com_util/mock_com_util.cc`

## 関数本体の実装

### 通常関数

```cpp
#include <testfw.h>
#include <mock_<lib>.h>

WEAK_ATR <rettype> <func>(<args>)
{
    <rettype> rtc = <default>;

    if (_mock_<lib> != nullptr)
    {
        rtc = _mock_<lib>-><func>(<args>);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s <入力引数>", __func__, <入力引数>...);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" -> <戻り値>\n", <値>);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
```

- 未注入時は `<default>` のまま返します。
- `delegate_real_` 呼び出しは使いません。
- トレースは `__func__` を先頭に出します。

参照先:
- `app/com_util/test/libsrc/mock_com_util/crt/mock_com_util_access.cc`
- `app/com_util/test/libsrc/mock_com_util/crt/mock_com_util_stat.cc`
- `app/com_util/test/libsrc/mock_com_util/crt/mock_com_util_fopen.cc`
- `app/com_util/test/libsrc/mock_com_util/crt/mock_com_util_gmtime.cc`

### `sscanf` 系

可変長引数入力系は `va_list` をそのまま Mock クラスへ渡します。

```cpp
WEAK_ATR int <func>(const char *buffer, const char *format, ...)
{
    int rtc = 0;
    va_list args;

    va_start(args, format);

    if (_mock_<lib> != nullptr)
    {
        rtc = _mock_<lib>-><func>(buffer, format, args);
    }

    va_end(args);

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s \"%s\", \"%s\"", __func__, buffer, format);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" -> %d\n", rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
```

`vsscanf` 版は `va_list` を引数で直接受け取ります。

参照先:
- `app/com_util/test/libsrc/mock_com_util/crt/mock_com_util_sscanf.cc`
- `app/com_util/test/libsrc/mock_com_util/crt/mock_com_util_vsscanf.cc`

## トレース出力

- `TRACE_INFO` では入力引数を表示します。
- `TRACE_DETAIL` では戻り値を `->` の後ろに表示します。
- ポインターは `0x%p` で表示します。
- `struct tm *` などの出力引数の中身までは、`crt` 配下の参照実装では表示していません。まずは既存パターンに合わせます。

## プラットフォーム分岐

app 向けで OS 分岐が必要な場合は、`_WIN32` を直接使わず `app/com_util/prod/include/com_util/base/platform.h` の統一マクロを使います。

```c
#if defined(PLATFORM_LINUX)
    /* Linux 向け処理 */
#elif defined(PLATFORM_WINDOWS)
    /* Windows 向け処理 */
#endif
```

## テストでの利用

Fixture 内で `Mock_<lib>` を生成すると、コンストラクターで `_mock_<lib>` が差し替わります。

```cpp
TEST_F(MyTest, example)
{
    Mock_com_util mock;

    EXPECT_CALL(mock, com_util_access(_, _))
        .WillOnce(Return(0));

    /* テスト対象コード */
}
```

呼び出し回数だけ確認したい場合は `EXPECT_CALL(...).Times(n)` とし、戻り値は `ON_CALL` の既定値を使います。

## 実装時の確認項目

- 追加した関数が `Mock_<lib>` に登録されていること
- `ON_CALL` の既定値が失敗側になっていること
- 関数本体に `WEAK_ATR` が付いていること
- 未注入時に本物へ委譲していないこと
- `sscanf` 系で `va_start` / `va_end` の範囲が正しいこと
- `testfw` 向けの `file`, `line`, `func` や `delegate_real_` を混在させていないこと

`mock_com_util` だけは上記確認項目のうち既定値と未注入時動作が異なり、
全関数で real delegate を持つことを前提に `create-mock-com-mock` の確認項目を使います。
