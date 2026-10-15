# ソース スタイル維持ガイドライン

## 概要

対象ワークスペースでは、`.gitattributes`、`.editorconfig`、`.clang-format` を併用してソース スタイルを維持します。

| 定義 | 主な役割 | 守備範囲 |
| --- | --- | --- |
| `.gitattributes` | Git 管理時の正規化 | リポジトリへ格納する改行コード、バイナリ判定 |
| `.editorconfig` | 編集時の基本設定 | 文字コード、改行コード、インデント、末尾空白 |
| `.clang-format` | C/C++ の構文整形 | 波括弧、折り返し、プリプロセッサ、マクロ整列など |

Table: スタイル定義ファイルの役割と守備範囲

これら 3 つの定義ファイルは、開発の異なる段階で連携して機能します。`.gitattributes` は Git 格納時の差分を防止し、`.editorconfig` はエディター保存時の入力を整え、`.clang-format` は C/C++ コードの構文配置を確定します。いずれか 1 つだけでは一貫したスタイルを維持できません。

## .gitattributes

`.gitattributes` では、Git がリポジトリへ格納・チェックアウトする際のテキスト属性を定義しています。

- **テキスト ファイル**: 原則として改行コードを LF で管理します (`text=auto eol=lf`)。
- **Windows 専用スクリプト**: `*.bat` および `*.cmd` は CRLF で管理します (`eol=crlf`)。
- **バイナリ ファイル**: 画像、圧縮ファイル、PDF、Draw.io 由来の SVG はバイナリとして扱います。

エディター保存時の挙動を案内する `.editorconfig` に対し、`.gitattributes` は Git リポジトリへコミットされる内容を強制的に正規化します。

### git config との関係

対象ワークスペースでは、属性定義 (`text` や `eol`) が改行コードを直接制御するため、`core.autocrlf` や `core.eol` などの利用者環境の設定に依存せず、リポジトリ側の定義が常に優先されます。

ただし、属性定義のない外部リポジトリとの作業や予期せぬ改行変換を防ぐため、利用者側の `git config` では次の設定を推奨します。

```bash
git config --global core.autocrlf false
```

### 新規ファイル種別の追加方針

現在の `.gitattributes` に該当しない新しい拡張子やファイル種別を追加する場合は、テキストかバイナリかの判定、および改行コードの要件を確認してください。必要に応じて `.gitattributes` に定義を追加し、リポジトリ側で方針を明示します。

## .editorconfig

`.editorconfig` では、エディターがファイルを保存する際の共通書式と、ファイル種別ごとの個別設定を定義しています。

### 基本設定

- **文字コード**: UTF-8
- **改行コード**: LF
- **インデント**: 4 スペース
- **末尾空白**: 原則として行末スペースを削除 (Markdown ファイルのみ強制改行用の末尾空白を保持)

### ファイル種別ごとの個別設定

- **Makefile 関連**: インデントにタブを使用
- **Windows 専用スクリプト (`*.bat`, `*.cmd`)**: 改行コードに CRLF を使用
- **PowerShell スクリプト (`*.ps1`)**: UTF-8 BOM を使用
- **メッセージ コンパイラ入力 (`*.mc`)**: UTF-8 BOM を使用

> [!NOTE]
> `.mc` ファイルに UTF-8 BOM を付与するのは、`mc.exe` が BOM のない入力をシステムの既定コード ページ (Windows 日本語環境では CP932) として読み込み、UTF-8 で記述された日本語コメントが文字化けすることを防ぐためです。

### VS Code での利用方法

VS Code で `.editorconfig` を反映するには、以下の拡張機能をインストールします。

| 項目 | 内容 |
| --- | --- |
| 名前 | EditorConfig |
| ID | `EditorConfig.EditorConfig` |
| 説明 | EditorConfig Support for Visual Studio Code |
| パブリッシャー | EditorConfig |
| VS Marketplace | [EditorConfig](https://marketplace.visualstudio.com/items?itemName=EditorConfig.EditorConfig) |

Table: VS Code 用 EditorConfig 拡張機能情報

## .clang-format

`.clang-format` では、C/C++ ソース コードの構文整形規則を定義しています。

- **基本構文**: LLVM スタイルをベースとし、インデント幅 4 スペース、折り返し幅 120 文字、波括弧は Allman スタイルを採用
- **改行コード**: LF
- **プリプロセッサ**: ネスト構造を可視化するため、ディレクティブの字下げに `BeforeHash` を採用
- **マクロ定義**: 定数定義を一覧しやすくするため、連続するマクロ定義を `Consecutive` で整列
- **インクルード順序**: `include` の自動並べ替えは無効化

### 整形結果の確定と手作業修正の禁止

C/C++ の空白、改行位置、桁揃えは `.clang-format` の出力を正とします。

- **手作業修正の禁止**: clang-format が出力した整形結果を手作業で変更したり、元の記述へ戻したりしません。
- **除外指定と理由コメント**: 整形を適用しない範囲は `// clang-format off` と `// clang-format on` で囲み、対象外とする理由をコメントに残します。
- **可読性低下時の対処**: 整形結果の可読性に問題がある場合は、ソースを手直しするのではなく、`.clang-format` の設定変更として提案します。

> [!NOTE]
> clang-format の出力は再実行に対して安定しており、整形済みのソースへ同じ設定で再度適用しても差分は出ません。手作業で字下げや桁揃えを修正しても次の整形で上書きされるため、個別対処ではなく設定側で解決します。
>
> 意図した整形が保たれないように見える代表例として、以下があります。いずれも clang-format の仕様どおりの挙動です。
>
> - 行末コメントに続けて書いた単独のコメント行が、コメント列ではなく直前の文の字下げ位置へ移動する。
> - プリプロセッサ ブロック内の単独コメントが、ブロックの字下げ位置へ移動する。
> - 1 行に収まるようになったマクロ定義から、不要になった行継続の `\` が削除される。

## NBSP チェック

NBSP (`U+00A0`) の混入は、`.editorconfig` や `.clang-format` では確実に抑止できません。  
対象ワークスペースでは、`app/general/bin/check-nbsp.py` を用いて Git 管理下のテキスト ファイルに NBSP が含まれていないことを検証します。

```bash
# ローカル環境: 新規追加・変更済みファイルのみを検証
make check-nbsp

# CI / Jenkins 環境: 管理対象の全ファイルを強制検証
python3 app/general/bin/check-nbsp.py --force
```

## 作業チェックリスト

ソース コードの作成・変更時は、以下の順序でスタイルを確認します。

1. **エディター環境の準備**: `.editorconfig` 拡張機能が有効になっていることを確認します。
2. **C/C++ の構文整形**: コード変更後、`.clang-format` に従って自動整形を実行します。整形結果に対する手作業の微修正は行いません。
3. **不可視文字の検証**: コミット前に `make check-nbsp` を実行し、NBSP が混入していないことを確認します。
4. **改行コードの整合**: `.gitattributes` の定義に従い、OS 固有の改行変換設定に頼らずコミットします。

## 関連ファイル

- `.gitattributes`
- `.editorconfig`
- `.clang-format`
- `app/general/bin/check-nbsp.py`

## 参考資料

- [Git - gitattributes Documentation](https://git-scm.com/docs/gitattributes)
- [Git - git-config Documentation](https://git-scm.com/docs/git-config)
