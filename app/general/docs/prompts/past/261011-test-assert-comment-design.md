# テストの確認コメントを判定の直接説明にする 最終仕様

検討 ID: test-assert-comment

版: 2

状態: 承認済み (2026-10-12 Tetsuo Honda)

上流文書: `prompt/test-assert-comment-discussion.md` (版 13)

## 目的

テスト コードの確認を、次の 3 点で整えます。

1. 確認タグに続く説明文 (以下「確認コメント」) を、マクロが行う判定をそのまま日本語で表す書き方に統一します。判定から導かれる総合的な期待 (以下「総合期待」) は、判定だけでは意図が読み取れない場合に限り、判定に続けて添えます。
2. Act フェーズの手順 (関数を呼び出して戻り値を変数で受ける) と、Assert フェーズの確認 (戻り値を判定する) を分けます。
3. `[状態確認]` を「テスト対象の振る舞いの確認に直接寄与しない確認」と定義し直し、確認件数がそのテストで検証したい振る舞いの数を表すようにします。

テストの手順と確認の説明は、製品コードのコメントと異なり、コードが何をしているかの直接的な説明を主体にします。

> [!NOTE]
> 確認コメントは、テスト結果 `results.md` の確認節に箇条書きで転記されます。同じ `results.md` の「テスト コード」節にはテスト関数のコードが載るため、読み手はコメントとマクロを突き合わせて読みます。判定をそのまま書くと、この突き合わせが容易になります。

## 範囲

### 対象

Table: 変更の対象

| 対象 | 内容 |
| --- | --- |
| 確認コメント | `[確認_正常系]`、`[確認_異常系]`、`[Pre-Assert確認_正常系]`、`[Pre-Assert確認_異常系]`、`[状態確認]` の説明文 |
| Act と Assert の分離 | Act フェーズの中にある `ASSERT_*` / `EXPECT_*` の配置と、呼び出しと判定の分離 |
| 確認タグの種類 | `[確認_*]` から `[状態確認]` への再分類、確認タグのない判定へのタグの付与 |
| タグとフェーズ | フェーズに合わない確認タグと判定の配置 |
| Pre-Assert の記載順 | `[Pre-Assert確認_*]` と `[Pre-Assert手順]` の並び順 |
| テスト コード | 全 app (14 件) と `framework/testfw` の C/C++、.NET のテスト コード |
| 文書 | 規範とその例 (「[文書の更新](#文書の更新)」節)、確認件数の棚卸し文書の削除 |

2026-10-12 時点の確認タグ付き説明文の件数は次のとおりです。

Table: 対象と件数

| 対象 | 件数 |
| --- | ---: |
| `app/cplat` | 8,495 |
| `app/porter` | 777 |
| `app/struct-meta` | 409 |
| `app/string-catalog-sample` | 250 |
| `app/cjson` | 74 |
| `app/calc.net` | 61 |
| `app/service-sample` | 53 |
| `app/calc` | 52 |
| `app/zlib` | 43 |
| `app/sqlite` | 34 |
| `app/lua` | 22 |
| `app/override-sample` | 21 |
| `app/tutorial` | 7 |
| `app/subfolder-sample` | 3 |
| `framework/testfw` | 8 |

すでにこの仕様に合っている箇所は変更しません。

### 対象外

- 確認タグの記法 (タグ名、`回数=` の式) と確認件数の集計規則
- `[状態]`、`[手順]`、`[Pre-Assert手順]` の説明文。ただし、[手順を説明していた確認](#手順を説明していた確認) で `[Pre-Assert手順]` を追加する場合を除きます。
- テスト関数の前に置く説明 (「〜ことの確認」)
- `framework/testfw` の生成処理 (`results.md` の出力形式)
- 外部由来の配布物と自動生成された成果物

## 確認タグの使い分け

Table: 確認タグの使い分け

| タグ | 意味 | 使うフェーズ | 確認件数 |
| --- | --- | --- | --- |
| `[確認_正常系]` / `[確認_異常系]` | テストの被テスト関数の振る舞いの判定 | Act、Assert | 含める |
| `[Pre-Assert確認_正常系]` / `[Pre-Assert確認_異常系]` | モックへの呼び出し期待の判定 | Pre-Assert、Cleanup (後処理の呼び出し期待)、モックのコールバック内 | 含める |
| `[状態確認]` | テスト対象の振る舞いの確認に直接寄与しない確認 | Arrange、Act | 含めない |

### 被テスト関数の判定と、直接寄与しない確認

被テスト関数は、テスト名とテスト関数の前の「〜ことの確認」が示す関数です。テスト内のヘルパー関数を通して呼ぶ場合も含みます。

- 被テスト関数の戻り値、出力、呼び出しによる状態の変化の判定は `[確認_*]` とします。
- それ以外の関数 (準備や結果の取り出しに使う補助の関数、ファイル操作、テスト用ヘルパー) の成否の判定は `[状態確認]` とします。

```cpp
// テスト対象は load()。cplat_string_catalog_filter_decompile_line() は結果を取り出す補助
// Act
int actual_ret = load(path_); // [手順] - ファイルを読み込む。
int actual_ret_decompile = cplat_string_catalog_filter_decompile_line(image_, sizeof(image_), 0U, actual_first,
                                                                      sizeof(actual_first)); // [手順] - 1 行目を復元する。
ASSERT_EQ(CPLAT_OK, actual_ret_decompile); // [状態確認] - cplat_string_catalog_filter_decompile_line() の戻り値が CPLAT_OK であること。

// Assert
EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - load() の戻り値が CPLAT_OK であること。(無効な行があっても読み込みに成功したことの確認)
```

> [!NOTE]
> `[状態確認]` は `results.md` の確認節に括弧で囲んで出力され、確認件数に含まれません。testfw の生成処理は `[状態確認]` をフェーズで区別しないため、生成処理の変更は不要です。

被テスト関数の特定に迷う箇所は、書き換えの際に列挙し、ユーザーに確認します。

### 確認タグのない判定

確認タグのない `ASSERT_*` / `EXPECT_*` が見つかった場合は、上の使い分けに従って確認タグを付けます。

## Act と Assert の分離

被テスト関数の戻り値は Act フェーズで変数に受け、Assert フェーズでその変数を判定します。

### Act の中の EXPECT

Act の中の `EXPECT_*` は、Assert フェーズへ移します。関数呼び出しを直接判定している場合は、戻り値を変数で受けます。

```cpp
// 変更前
// Act
int actual_ret_tracer_write = cplat_tracer_write_at(…); // [手順] - VERBOSE を stderr へ出力する。
EXPECT_EQ(CPLAT_OK, actual_ret_tracer_write);           // [確認_正常系] - …
int actual_ret_tracer_write_2 = cplat_tracer_write_at(…); // [手順] - DEBUG を stderr へ出力する。

// 変更後
// Act
int actual_ret_tracer_write = cplat_tracer_write_at(…);   // [手順] - VERBOSE を stderr へ出力する。
int actual_ret_tracer_write_2 = cplat_tracer_write_at(…); // [手順] - DEBUG を stderr へ出力する。

// Assert
EXPECT_EQ(CPLAT_OK, actual_ret_tracer_write);   // [確認_正常系] - VERBOSE を出力した cplat_tracer_write_at() の戻り値が CPLAT_OK であること。
EXPECT_EQ(CPLAT_OK, actual_ret_tracer_write_2); // [確認_正常系] - DEBUG を出力した cplat_tracer_write_at() の戻り値が CPLAT_OK であること。
```

ループの中の `EXPECT_*` は、反復ごとの値を配列などに残し、Assert で確かめます。

### Act の中の ASSERT

Assert フェーズは、被テスト関数の判定だけとします。Act の中の `ASSERT_*` は、判定の種類と後続の Act によって扱いを分けます。

Table: Act の中の ASSERT の扱い

| 判定 | 後続 | 扱い |
| --- | --- | --- |
| 直接寄与しない確認 (`[状態確認]`) | 問わない | Act に残し、呼び出し (`[手順]`) と判定を別の文に分ける |
| 被テスト関数の判定 (`[確認_*]`) | 有意な Act がある | Act に残し、呼び出しと判定を別の文に分ける |
| 被テスト関数の判定 (`[確認_*]`) | 有意な Act がない | Assert へ移す。Assert の先頭側に、元の順序を保って置く |
| ループの中の判定 | 問わない | Act に残し、呼び出しと判定を別の文に分ける |

有意な Act は、判定した呼び出しの結果 (戻り値、出力引数、状態) に依存する関数の呼び出しです。値を変数へ退避する文や後始末は含みません。

```cpp
// 直接寄与しない確認 (Act に残し、呼び出しと判定を分ける)
// Act
FILE *out = cplat_fopen(path.c_str(), "wb", NULL);   // [手順] - 書き出し先を開く。
ASSERT_NE(nullptr, out);                             // [状態確認] - cplat_fopen() の戻り値が nullptr 以外であること。
size_t wrote_mgmt = fwrite(mgmt, 1, mgmt_size, out); // [手順] - 管理領域を書き出す。
```

```cpp
// 被テスト関数の判定で、後続に有意な Act がある場合 (Act に残し、呼び出しと判定を分ける)
// テスト対象は cplat_hashtable_add()。追加の成否に依存して検索する
// Act
int actual_ret_add = cplat_hashtable_add(ht, "a", value.data(), …); // [手順] - キーを追加する。
ASSERT_EQ(CPLAT_OK, actual_ret_add); // [確認_正常系] - cplat_hashtable_add() の戻り値が CPLAT_OK であること。
int actual_ret_find = cplat_hashtable_find_value_ref(ht, "a", &ref); // [手順] - 追加したキーを検索する。
```

```cpp
// 被テスト関数の判定で、後続に有意な Act がない場合 (Assert へ移す)
// Act
int actual_ret = f(…); // [手順] - …

// Assert
ASSERT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - f() の戻り値が CPLAT_OK であること。
```

### タグとフェーズの不一致

確認タグとフェーズの組み合わせが [確認タグの使い分け](#確認タグの使い分け) に合わない箇所は、判定を正しいフェーズへ移すか、タグをフェーズに合わせて変えます。どちらにするかは、この節と [被テスト関数の判定と、直接寄与しない確認](#被テスト関数の判定と直接寄与しない確認) を当てはめて決めます。

Table: タグとフェーズの不一致

| フェーズ | タグ | 件数 (2026-10-12 時点の概数) |
| --- | --- | ---: |
| Pre-Assert | `[確認_*]` | 74 |
| Pre-Assert | `[状態確認]` | 18 |
| Arrange | `[Pre-Assert確認_*]` | 22 |
| Act | `[Pre-Assert確認_*]` | 5 |
| Assert | `[状態確認]` | 5 |
| Cleanup | `[確認_*]` | 5 |

## 確認コメントの書き方

### 構成

確認コメントは、判定 (1 文目) と、必要な場合の総合期待で構成します。

```text
// [確認_正常系] - <判定>。(<総合期待>ことの確認)
```

- 判定は「〜であること。」などの形で書き、句点で終えます。
- 総合期待は、判定の句点の直後に空白を入れずに半角の括弧で囲み、「〜ことの確認」で終えます。括弧の後に句点は付けません。
- 総合期待がない場合は、判定の句点で終えます。

### 判定 (1 文目)

判定は、確認すべき値を主語にし、期待値とどのような関係かを簡潔に表します。

- 関数の戻り値は「f() の戻り値」と書きます (f は関数名)。戻り値を変数で受けてから判定する場合も、マクロの引数に関数呼び出しを直接書く場合も同じです。両辺が関数の戻り値の場合は、両辺とも「f() の戻り値」と書きます。
- メソッドの戻り値は、オブジェクト名を含めて書きます (例: 「results.size() の戻り値」)。
- テスト内のヘルパー関数を通して呼ぶ場合は、テストが呼ぶヘルパー関数の名前を書きます。
- 関数の戻り値以外 (出力引数、ループで作った変数、構造体のメンバーなど) は、変数名や式をそのまま書きます。
- 同じ関数を時点や入力を変えて呼び分けている場合は、その条件を主語の前に置きます (例: 「追加後の …」「3 バイトのオーバー ロング表現に対する …」)。判定の対象を特定する引数 (探す文字列など) も、主語の前に置きます (例: 「"キューの長さは 7 です。" を探した captured.find() の戻り値が …」)。
- 関数名の後には `()` を付け、引数は書きません。
- 識別子と値はバッククォートで囲みません。
- 期待値の文字列リテラルは全文書きます。

マクロごとの書き方は次のとおりです。`ASSERT_*` は対応する `EXPECT_*` と同じです。

Table: マクロごとの判定の書き方

| マクロ | 判定の書き方 |
| --- | --- |
| `EXPECT_EQ(A, B)`、`EXPECT_DOUBLE_EQ(A, B)` | A が値 (リテラル、定数) なら「B が A であること。」、A が変数や式なら「B が A と一致すること。」 |
| `EXPECT_NE(A, B)` | 「B が A 以外であること。」 |
| `EXPECT_LT(A, B)` / `EXPECT_LE(A, B)` | 「A が B より小さいこと。」/「A が B 以下であること。」。A が期待値 (リテラル、定数) なら、B を主語にして「B が A より大きいこと。」/「B が A 以上であること。」 |
| `EXPECT_GT(A, B)` / `EXPECT_GE(A, B)` | 「A が B より大きいこと。」/「A が B 以上であること。」。A が期待値なら、B を主語にして「B が A より小さいこと。」/「B が A 以下であること。」 |
| `EXPECT_TRUE(X)` / `EXPECT_FALSE(X)` | 「X が true であること。」/「X が false であること。」 |
| `EXPECT_STREQ(A, B)` / `EXPECT_STRNE(A, B)` | 「B が A であること。」/「B が A 以外であること。」 |
| `EXPECT_THAT(V, M)` | 「V が <M の日本語> であること。」(M は「[引数の条件](#引数の条件)」の表で言い換える) |
| `ASSERT_NO_THROW(S)` | 「S が例外を送出しないこと。」(S が関数呼び出しなら「f() が例外を送出しないこと。」) |
| `ASSERT_NO_FATAL_FAILURE(f())` | 「f() の中で致命的な失敗が起きないこと。」 |
| `EXPECT_NONFATAL_FAILURE(S, T)` | 「<S の内容> が T を含む非致命的な失敗を報告すること。」 |
| `EXPECT_DEATH(S, R)` / `EXPECT_EXIT(S, P, R)` | 「f() がプロセスを終了させ、出力が R に一致すること。」(`EXPECT_EXIT` は終了条件 P も書く) |
| `SUCCEED()` | 「SUCCEED() に到達すること。」(直前の処理の意味は総合期待に書く) |
| `EXPECT_FILE_EXISTS(F)` | 「F のファイルが存在すること。」 |
| `EXPECT_FILE_CONTAINS(F, T)` | 「F のファイルが T を含むこと。」 |
| `Assert.Equal(A, B)` (.NET) | `EXPECT_EQ(A, B)` と同じ |
| `Assert.True(X)` / `Assert.False(X)` (.NET) | `EXPECT_TRUE(X)` / `EXPECT_FALSE(X)` と同じ |
| `Assert.Contains(A, B)` (.NET) | 「B が A を含むこと。」 |
| `Assert.Throws<E>(S)` (.NET) | 「S が E を送出すること。」 |
| `Assert.Null(X)` / `Assert.Same(A, B)` / `Assert.IsType<T>(X)` (.NET) | 「X が null であること。」/「B が A と同じインスタンスであること。」/「X の型が T であること。」 |

### 総合期待

総合期待は、判定だけでは意図が読み取れない場合に書きます。例えば次の場合です。

- 0 や 0 以外などの値が何を意味するか (例: 取り込んだか、取り込まなかったか)
- 期待値の数値の由来 (例: 0 から 1 増えた結果が 1 である)
- 比較の理由 (例: 時計を巻き戻したため実時刻が逆行する)
- `SUCCEED()` の直前の処理の意味 (例: NULL を渡した解放処理がクラッシュしない)

判定から自明な言い換え (例: 戻り値が CPLAT_OK であることに対する「成功したことの確認」) は書きません。  
`[状態確認]` には総合期待を書きません。

> [!NOTE]
> `results.md` では `[状態確認]` の項目を説明文全体ごと括弧で囲んで出力するため、総合期待を括弧で添えると括弧が入れ子になります。

### Pre-Assert の確認

`EXPECT_CALL` の `[Pre-Assert確認_*]` には、呼び出しの回数と引数の条件を判定として書きます。

#### 呼び出しの回数

Table: 呼び出し回数の書き方

| 指定 | 判定の書き方 |
| --- | --- |
| `.Times(0)` | 「f() が呼び出されないこと。」 |
| `.Times(n)`、または `WillOnce` が n 個で `WillRepeatedly` なし | 「f() が n 回呼び出されること。」 |
| `.Times(AtLeast(n))`、または `WillOnce` が n 個で `WillRepeatedly` あり | 「f() が n 回以上呼び出されること。」 |
| `.Times(AnyNumber())`、または `WillRepeatedly` だけ | 「f() が任意の回数呼び出されること。」 |

#### 引数の条件

`_` (任意の値) 以外の引数条件を、「X を指定して」の形で列挙します。mock オブジェクト名は書きません。

GoogleMock のマッチャーで指定した引数は、意味を日本語で書きます。

Table: マッチャーの言い換え

| マッチャー | 日本語 |
| --- | --- |
| `StrEq(x)` | x と等しい文字列 |
| `HasSubstr(x)` | x を含む文字列 |
| `NotNull()` | NULL 以外 |
| `Pointee(x)` | 指す先が x であるポインター |

上の表にないマッチャーも、同じ考え方で判定の意味を日本語で書きます。

#### 記載順

1 つの `EXPECT_CALL` に付ける説明は、`[Pre-Assert確認_*]` を先に、`[Pre-Assert手順]` を後に書きます。

```cpp
EXPECT_CALL(mock_cplat_, cplat_fopen(StrEq(path), StrEq("rb"), nullptr))
    .WillOnce(Return(file_)); // [Pre-Assert確認_正常系] - cplat_fopen() が path と等しい文字列、"rb" と等しい文字列、nullptr を指定して 1 回呼び出されること。
                              // [Pre-Assert手順] - 設定ファイルを開くハンドルを返す。
```

> [!NOTE]
> `results.md` では確認と手順が別の節に分かれて出力されるため、記載順は出力に影響しません。テスト コード上で「〜が呼び出されることを確認し、〜を返す」という構文の説明として読めるよう、可読性のためにこの順序とします。

#### 手順を説明していた確認

`[Pre-Assert確認_*]` が `WillOnce` の動作 (手順) を説明していた場合は、確認を呼び出し回数の判定に直し、動作の説明を `[Pre-Assert手順]` として追加します。

```cpp
EXPECT_CALL(mock_cplat_, cplat_calloc(_, _))
    .WillOnce(DoDefault())      // [Pre-Assert確認_異常系] - cplat_calloc() が 2 回呼び出されること。
                                // [Pre-Assert手順] - 1 回目 (管理領域+データ領域) は成功させる。
    .WillOnce(Return(nullptr)); // [Pre-Assert手順] - 2 回目 (内部管理データ) から NULL を返却する。
```

## 書き換えの例

Table: 書き換えの例

| 型 | 現状 | 書き換え後 |
| --- | --- | --- |
| 戻り値の判定を結果の解釈で書いている | `EXPECT_EQ(CPLAT_OK, actual_ret_added_table); // [確認_正常系] - 追加後にテーブル世代を読めること。` | `… // [確認_正常系] - 追加後の cplat_hashtable_get_table_generation() の戻り値が CPLAT_OK であること。` |
| 値の一致を変化量で書いている | `EXPECT_EQ(1u, added_table); // [確認_正常系] - 追加でテーブル世代が 1 増えること。` | `… // [確認_正常系] - added_table が 1 であること。(追加でテーブル世代が 0 から 1 増えたことの確認)` |
| 値の一致を否定形で書いている | `EXPECT_EQ(40u, after_small); // [確認_正常系] - 小さい世代を渡してもテーブル世代が戻らないこと。` | `… // [確認_正常系] - after_small が 40 であること。(世代 7 を渡した後もテーブル世代が戻らないことの確認)` |
| 大小比較を現象で書いている | `EXPECT_LT(second_time.tv_sec, first_time.tv_sec); // [確認_正常系] - 実時刻は時計の巻き戻しにより逆行すること。` | `… // [確認_正常系] - second_time.tv_sec が first_time.tv_sec より小さいこと。(時計の巻き戻しで実時刻が逆行したことの確認)` |
| 期待値が先に来る大小比較 | `EXPECT_LT(0U, strlen(actual_note)); // …` | `… // [確認_正常系] - strlen() の戻り値が 0U より大きいこと。(cplat_string_catalog_get_note() の戻り値が空でないことの確認)` |
| 値の意味だけを書いている | `EXPECT_NE(0, actual_after_release); // [確認_正常系] - 解放後の出力で取り込むこと。` | `… // [確認_正常系] - actual_after_release が 0 以外であること。(解放後の出力で条件を取り込んだことの確認)` |
| 探す文字列が判定から消える | `EXPECT_NE(std::string::npos, captured.find("キューの長さは 7 です。")); // …` | `… // [確認_正常系] - "キューの長さは 7 です。" を探した captured.find() の戻り値が std::string::npos 以外であること。(利用者の引数が展開されたことの確認)` |
| 文字列の一致に経緯を混ぜている | `EXPECT_STREQ("a", buf); // [確認_正常系] - 上矢印が無視されて "a" が返ること。` | `… // [確認_正常系] - buf が "a" であること。(上矢印が無視されたことの確認)` |
| 呼び出し回数を目的で書いている | `EXPECT_CALL(mock_zlib, deflate(_, _)).Times(0); // [Pre-Assert確認_異常系] - 初期化失敗後に処理しないこと。` | `… // [Pre-Assert確認_異常系] - deflate() が呼び出されないこと。(初期化に失敗した後は圧縮処理へ進まないことの確認)` |
| 真偽値の変数を意味だけで書いている | `EXPECT_TRUE(in_range); // [確認_正常系] - すべての番号が 1 から上限までに収まること。` | `… // [確認_正常系] - in_range が true であること。(すべての番号が 1 から SAMPLECATALOG_SEQUENCE_NUMBER_MAX までに収まることの確認)` |
| 入力条件だけで判定を書いていない | `EXPECT_FALSE(overlong_three_result); // [確認_異常系] - 3 バイトのオーバー ロング表現が拒否されること。` | `… // [確認_異常系] - 3 バイトのオーバー ロング表現に対する cplat_internal_regex_utf8_decode() の戻り値が false であること。` |
| 式全体を引用している | `` // [状態確認] - `compile_single_line("key == …", image)` の戻り値が `CPLAT_OK` であること。`` | `// [状態確認] - compile_single_line() の戻り値が CPLAT_OK であること。` |
| クラッシュしないことを示す | `SUCCEED(); // [確認_正常系] - NULL を渡しても dispose がクラッシュしないこと。` | `SUCCEED(); // [確認_正常系] - SUCCEED() に到達すること。(NULL を渡した cplat_hashtable_dispose() がクラッシュしないことの確認)` |

## 規範に追加する規則

上の各節に加え、次の規則を規範に記載します。経緯は記載しません。

Table: 規範に追加する規則

| 規則 | 記載先 |
| --- | --- |
| クラッシュしないことを示す場合は、コメントだけで済ませず `SUCCEED()` を記載して確認タグを付ける | `about-test-phase.md` の Assert フェーズ |
| モックのコールバック内の `ASSERT_*` / `EXPECT_*` には `[Pre-Assert確認_*]` を付ける | `about-test-phase.md` の Pre-Assert フェーズ |
| 実行環境によって個数が変わる確認 (errno の一覧など) は、期待する一覧と実際の一覧を 1 回で照合するなど、環境によらず確認回数が一定になる形にする | `about-test-phase.md` の回数指定の節 |
| ループ全体の説明と各判定の説明が重複する場合、全体の説明はタグを付けない通常のコメントにする | `about-test-phase.md` の回数指定の節 |
| 確認タグの「個数」は記載箇所の数、「確認件数」は回数式を評価した合計であり、区別する | `about-test-phase.md` の回数指定の節 |
| スクリプト (Python、JavaScript、PowerShell、シェル) のテストは各テスト ランナーの集計を使い、testfw の専用コメントによる件数集計の対象外とする | `framework/testfw/docs/how-to-test.md` |

## 文書の更新

### about-test-phase.md

`framework/testfw/docs/about-test-phase.md` に、この仕様の規則を記載します。

Table: about-test-phase.md の更新

| 箇所 | 更新内容 |
| --- | --- |
| 冒頭の概要と Arrange フェーズの `[状態確認]` | `[状態確認]` を「テスト対象の振る舞いの確認に直接寄与しない確認」と定義し直し、Arrange と Act で使うことと、被テスト関数の判定との区別を記載する |
| Pre-Assert フェーズ | 呼び出し回数、引数の条件、マッチャーの言い換え、記載順、手順を説明していた確認の扱い、モックのコールバック内の判定のタグ |
| Act フェーズ | Act と Assert の分離 (Act の中の `EXPECT_*` と `ASSERT_*` の扱い、有意な Act) |
| Assert フェーズ | 確認コメントの書き方 (構成、判定の原則、主語、マクロごとの書き方、総合期待)、`SUCCEED()` の扱い |
| 回数指定の節 | 実行環境によって個数が変わる確認、ループ全体の説明、確認タグの個数と確認件数の区別 |
| 例 | 文書内の確認コメントの例を、この仕様の書き方に揃える |

> [!NOTE]
> 最終仕様 版 1 の承認後に、`about-test-phase.md` へ確認コメントの書き方の大部分 (構成、主語、マクロごとの書き方、Pre-Assert の回数、引数、マッチャー、記載順) を記載済みです。版 2 では、上の表のうち追加分を記載します。

### その他の文書

Table: その他の文書の更新

| 文書 | 更新内容 |
| --- | --- |
| `framework/testfw/docs/how-to-test.md` | スクリプトのテストの扱い |
| `app/general/docs/testing-tutorial.md`、`framework/testfw/docs/test-subprocedures.md`、`framework/makefw/docs/subfolder-compilation.md`、`app/tutorial/test/src/static-access/README.md` | 確認コメントの例を、この仕様の書き方に揃える (版 1 の承認後に実施済み。版 2 の規則に合わない箇所があれば直す) |
| `app/general/docs/test-confirmation-count-inventory.md`、`app/general/docs/test-confirmation-count-inventory.tsv` | 削除する (`rm` を使い、`git rm` は使わない) |
| `app/general/docs/README.md` | 棚卸し文書へのリンクを削除する |

## 作業の進め方

1. 文書を更新し、棚卸し文書を削除します。
2. 見本 (`app/string-catalog-sample` と `app/cplat/test/src/libcplatTest/hashtable/`) を、この仕様に合わせて直します。見本は最終仕様 版 1 で説明文を書き換え済みのため、版 2 で加わった規則 (判定の原則、Act と Assert の分離、再分類、フェーズの直し、Pre-Assert の追加規則) を適用します。
3. 見本の差分と、被テスト関数の特定などで迷った箇所の一覧をユーザーに提示し、確認を受けます。指摘があれば規則または書き換えを直し、確認を受け直します。
4. 残りの app と `framework/testfw` を書き換えます。

- 書き換えは Sonnet のサブエージェントに app またはディレクトリの単位で分担させます。Claude は依頼文の作成、差分のレビュー、検証を行います。
- サブエージェントには担当範囲以外のファイルを変更させず、`make` と `clean` を実行させません。
- `make` は Claude が 1 本ずつ実行します。

## 検証

Table: 検証の方法

| 検証 | 方法 |
| --- | --- |
| 確認件数の照合 | テスト関数ごとに、確認タグを区分 (正常系、異常系) と回数式で集計し、変更前 (各リポジトリの `HEAD`) と比べる。差が、再分類、タグの付与、フェーズの直しで意図した分だけであることを確認する |
| 解析と実行 | 各 app と `framework/testfw` の `make test` を 1 本ずつ実行し、テストが成功し、確認コメントの解析エラーがないことを確認する |
| 警告 | `make test` 後に、対象範囲の内容がある `.warn` ファイルを確認する |
| ソースの整形 | 変更したソースに `git clang-format` を適用し、差分がなくなることを確認する |
| タグの漏れ | 変更したテスト ソースに `text_style_jp.py --mode cpp --dry-run` を実行し、確認コメントの不足がないことを確認する |
| 文書の整形 | 変更した Markdown ごとに `text_style_jp.py --dry-run` を実行し、問題がなければ `--in-place` を実行する |
| 規則への適合 | 差分をレビューし、この仕様に合わない説明文、Act の中の `EXPECT_*`、逆順の `EXPECT_CALL`、フェーズに合わないタグが残っていないことを確認する |

## 受入条件

- `about-test-phase.md` と `how-to-test.md` に、この仕様の規則と例が記載されていること。
- 例を載せた他の文書 4 件の例が、この仕様の書き方に揃っていること。
- 棚卸し文書 2 件が削除され、`app/general/docs/README.md` からのリンクがないこと。
- 見本の書き換えについて、ユーザーの確認を受けていること。
- 対象のテスト コードの確認コメントが、この仕様の書き方に揃っていること。
- Act の中に `EXPECT_*` が残っていないこと。Act の中の `ASSERT_*` は、直接寄与しない確認、後続に有意な Act がある被テスト関数の判定、またはループの中の判定であり、呼び出しと判定が別の文に分かれていること。
- Assert フェーズの判定が、被テスト関数の判定だけであること。
- `[Pre-Assert手順]` が `[Pre-Assert確認_*]` より先に書かれた `EXPECT_CALL` が残っていないこと。
- 確認タグとフェーズの組み合わせが [確認タグの使い分け](#確認タグの使い分け) に合っていること。
- テスト関数ごとの確認件数の変更前との差が、意図した分だけであること。
- 各 app と `framework/testfw` の `make test` が成功し、`.warn` に今回の変更による警告がないこと。
- 変更したソースが `git clang-format` で差分なしとなり、`text_style_jp.py --mode cpp --dry-run` で確認コメントの不足がないこと。
- 変更した Markdown が `text_style_jp.py --dry-run` で問題なしとなること。
