# ソース スタイル維持ガイドライン

## 概要

このリポジトリでは、`.gitattributes`、`.editorconfig`、`.clang-format` を併用してソース スタイルを維持します。

| 定義 | 主な役割 | 守備範囲 |
| --- | --- | --- |
| `.gitattributes` | Git 管理時の正規化 | リポジトリへ格納する改行コード、バイナリ判定 |
| `.editorconfig` | 編集時の基本設定 | 文字コード、改行コード、インデント、末尾空白 |
| `.clang-format` | C/C++ の構文整形 | 波括弧、折り返し、プリプロセッサ、マクロ整列など |

3 つは同じ目的を異なる段階で支えます。`.gitattributes` は Git に保存される内容をそろえ、`.editorconfig` は編集時の入力をそろえ、`.clang-format` は C/C++ コードの見た目をそろえます。いずれか 1 つだけでは守備範囲が不足します。

## .gitattributes

`.gitattributes` では、主に以下を定義しています。

- テキスト ファイルは原則として LF で管理する
- Windows 専用の `*.bat` と `*.cmd` は CRLF で管理する
- 画像、圧縮ファイル、PDF、Draw.io 由来の SVG はバイナリとして扱う

`.editorconfig` と同じく改行コードに関与しますが、役割は異なります。`.editorconfig` はエディターが保存する内容を案内し、`.gitattributes` は Git がリポジトリへ格納する内容を正規化します。

### git config との関係

このリポジトリでは、通常のテキスト ファイルに `text=auto eol=lf` を指定し、Windows 専用スクリプトには `eol=crlf` を個別指定しています。`text` や `eol` を `.gitattributes` で明示したパスでは、`core.autocrlf` や `core.eol` ではなく属性定義が改行コードを制御します。そのため、このリポジトリの管理対象ファイルは、利用者ごとの `git config` の影響を受けにくく、リポジトリ側の定義が優先されます。

利用者側の `git config` は、次の設定を推奨します。

```bash
git config --global core.autocrlf false
```

このリポジトリでは改行コードの方針を `.gitattributes` に集約しているため、利用者ごとの自動変換設定には依存しません。`core.autocrlf=false` としておくと、属性定義のない別リポジトリでも Git による暗黙の改行変換を避けやすくなります。

将来、現在の `.gitattributes` に該当しない新しいファイル種別を追加する場合は、そのファイルをテキストとして扱うか、改行コードをどう扱うか、バイナリとして固定するかを見直してください。必要に応じて `.gitattributes` を追加更新し、リポジトリ側で方針を明示します。

## .editorconfig

`.editorconfig` では、主に以下を定義しています。

- 既定の文字コードは UTF-8
- 既定の改行コードは LF
- 既定のインデントは 4 スペース
- 行末空白は原則として削除
- Markdown では意図的な行末スペースを保持
- Makefile 系ファイルではタブを使用
- Windows 専用スクリプトでは CRLF を使用
- PowerShell スクリプトでは UTF-8 BOM を使用

VS Code で `.editorconfig` を反映するには、以下の拡張機能をインストールしてください。

| 項目 | 内容 |
| --- | --- |
| 名前 | EditorConfig |
| ID | `EditorConfig.EditorConfig` |
| 説明 | EditorConfig Support for Visual Studio Code |
| パブリッシャー | EditorConfig |
| VS Marketplace | [EditorConfig](https://marketplace.visualstudio.com/items?itemName=EditorConfig.EditorConfig) |

## .clang-format

`.clang-format` では、主に以下を定義しています。

- ベース スタイルは LLVM
- 改行コードは LF
- インデント幅は 4 スペース
- 折り返し幅は 120 文字
- 波括弧は Allman スタイル
- プリプロセッサ ディレクティブは `BeforeHash`
- 連続するマクロ定義は `Consecutive` で整列
- `include` の自動並べ替えは行わない

`BeforeHash` は、ネストしたプリプロセッサ ディレクティブの構造を読み取りやすくするために採用しています。  
`Consecutive` は、定数定義を一覧しやすくするために採用しています。

## 編集時の基本方針

- Git 管理上の改行コードは `.gitattributes` に従う
- エディターでは `.editorconfig` を有効にする
- C/C++ ソースを変更した後は `.clang-format` に従って整形する
- 新規ファイルでは既存設定に従い、改行コードやインデントを個別判断で変えない

## 関連ファイル

- `.gitattributes`
- `.editorconfig`
- `.clang-format`

## 参考資料

- [Git - gitattributes Documentation](https://git-scm.com/docs/gitattributes)
- [Git - git-config Documentation](https://git-scm.com/docs/git-config)
