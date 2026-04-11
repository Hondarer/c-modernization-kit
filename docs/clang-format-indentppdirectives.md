# clang-format 設定の解説

## 概要

本プロジェクトでは `.clang-format` により、C/C++ ソースコードのフォーマットルールを定義しています。
このドキュメントでは、各オプションの設定意図とデフォルト値との違いを説明します。

```yaml
# .clang-format
BasedOnStyle: LLVM
IndentPPDirectives: BeforeHash
ColumnLimit: 120
IndentWidth: 4
AlignConsecutiveMacros: Consecutive
```

## オプションの値

`IndentPPDirectives` には以下の 3 つの値を指定できます。

### None (デフォルト)

ディレクティブをインデントしません。すべてのディレクティブが行頭に配置されます。

```c
void func() {
    if (condition) {
#ifdef DEBUG
#if defined(VERBOSE)
        printf("debug\n");
#endif
#endif
    }
}
```

### AfterHash (Clang 6 で導入)

`#` は行頭に固定し、`#` の後にインデントを挿入します。

```c
void func() {
    if (condition) {
#  ifdef DEBUG
#    if defined(VERBOSE)
        printf("debug\n");
#    endif
#  endif
    }
}
```

### BeforeHash (Clang 9 で導入)

`#` ごとインデントします。ネストの深さが視覚的にわかりやすくなります。

```c
void func() {
    if (condition) {
        #ifdef DEBUG
            #if defined(VERBOSE)
        printf("debug\n");
            #endif
        #endif
    }
}
```

## デフォルトが None である歴史的経緯

### C 言語の仕様上の制約 (1970年代〜1989年)

K&R C および初期の C コンパイラでは、`#` は行の先頭カラムに置かなければならないという制約がありました。この制約により、プリプロセッサディレクティブを行頭に書くスタイルが定着しました。

ANSI C (C89) 以降、`#` の前に空白を置くことが規格上許容されるようになりましたが、慣習は変わりませんでした。

### プリプロセッサの独立性という設計思想

C 言語においてプリプロセッサは、コンパイラ本体とは独立したフェーズとして設計されています。プリプロセッサディレクティブはソースコードの構文構造 (関数やブロック) とは別の次元で処理されるため、コードのインデント階層に従属させないという思想がありました。

```c
/* ディレクティブはコードの構造とは独立した存在 */
void func() {
    int x = 0;
#ifdef FEATURE_A        /* コードブロックの中だが行頭に書く */
    x = calculate();
#else
    x = default_value();
#endif
    return x;
}
```

### 主要プロジェクトのコーディングスタイル

影響力のある大規模プロジェクトがディレクティブを行頭に書くスタイルを採用しており、これが業界標準として定着しました。

- Linux カーネル: [Linux kernel coding style](https://www.kernel.org/doc/html/latest/process/coding-style.html) ではディレクティブを行頭に配置
- LLVM/Clang: clang-format の開発元である LLVM プロジェクト自体がこのスタイルを採用
- GNU プロジェクト: GCC をはじめとする GNU ソフトウェアも同様

clang-format のデフォルト値は LLVM のコーディングスタイルに基づいているため、`None` がデフォルトになっています。

### 既存コードベースへの影響の最小化

デフォルトを `BeforeHash` や `AfterHash` にした場合、clang-format を既存プロジェクトに適用すると大量のプリプロセッサ行に差分が発生します。既存のコードベースの大多数がディレクティブを行頭に書くスタイルであるため、`None` をデフォルトにすることで既存コードへの影響を最小限に抑えています。

### オプションの追加時期

`IndentPPDirectives` オプション自体が比較的新しい機能です。

| バージョン          | 追加内容                                                    |
| ------------------- | ----------------------------------------------------------- |
| Clang 6 (2018年3月) | `IndentPPDirectives` オプション導入、`AfterHash` をサポート |
| Clang 9 (2019年9月) | `BeforeHash` をサポート                                     |

後から追加されたオプションであるため、既存のデフォルト動作 (`None`) を変更する理由がありませんでした。

## BeforeHash 採用のメリット

C89 以降であれば、BeforeHash を採用することが推奨されます。その理由は以下の通りです。

1. 可読性の向上: ディレクティブのネスト構造が視覚的にわかりやすくなる
2. コードとの一体感: ディレクティブが周囲のコードと同じインデント階層に属するため、コードの流れを追いやすい

## AlignConsecutiveMacros

### オプションの説明

`AlignConsecutiveMacros` は、連続する `#define` マクロの値やコメントの位置を縦に揃えるかどうかを制御します。デフォルト値は `None` (揃えない) です。

### Consecutive (本プロジェクトの設定)

連続するマクロ定義の値の開始位置を、最長のマクロ名に合わせて揃えます。空行やコメント行で区切られたグループごとに揃えます。

```c
#define CALC_KIND_ADD      1 /**< 加算の演算種別を表す定数。 */
#define CALC_KIND_SUBTRACT 2 /**< 減算の演算種別を表す定数。 */
#define CALC_KIND_MULTIPLY 3 /**< 乗算の演算種別を表す定数。 */
#define CALC_KIND_DIVIDE   4 /**< 除算の演算種別を表す定数。 */
```

### None (デフォルト)

マクロ名の直後に値が配置され、位置は揃えられません。

```c
#define CALC_KIND_ADD 1 /**< 加算の演算種別を表す定数。 */
#define CALC_KIND_SUBTRACT 2 /**< 減算の演算種別を表す定数。 */
#define CALC_KIND_MULTIPLY 3 /**< 乗算の演算種別を表す定数。 */
#define CALC_KIND_DIVIDE 4 /**< 除算の演算種別を表す定数。 */
```

### デフォルトが None である理由

`None` がデフォルトである主な理由は、git diff への影響です。`Consecutive` を指定した場合、新しいマクロの追加や既存マクロのリネームにより最長のマクロ名が変わると、グループ内の全行でアライメントが再調整されます。これにより、本質的な変更が1行であっても、git diff 上では複数行の変更として表示されます。

```diff
 /* CALC_KIND_MULTIPLICATION を追加した場合の差分 */
-#define CALC_KIND_ADD      1
-#define CALC_KIND_SUBTRACT 2
-#define CALC_KIND_MULTIPLY 3
-#define CALC_KIND_DIVIDE   4
+#define CALC_KIND_ADD            1
+#define CALC_KIND_SUBTRACT       2
+#define CALC_KIND_MULTIPLY       3
+#define CALC_KIND_DIVIDE         4
+#define CALC_KIND_MULTIPLICATION 5
```

### 本プロジェクトで Consecutive を採用した理由

1. 定数定義の一覧性: 値とコメントが縦に揃うことで、定数テーブルとしての可読性が大幅に向上する
2. 定数の安定性: 定数定義は一度決めたら変更頻度が低いため、アライメント再調整による差分の影響は限定的である
3. レビュー効率: コードレビュー時に値の違いを一目で比較できる

## 参考資料

- [Clang-Format Style Options - IndentPPDirectives](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#indentppdirectives)
- [Clang-Format Style Options - AlignConsecutiveMacros](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#alignconsecutivemacros)
- [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html)
- [Linux kernel coding style](https://www.kernel.org/doc/html/latest/process/coding-style.html)
