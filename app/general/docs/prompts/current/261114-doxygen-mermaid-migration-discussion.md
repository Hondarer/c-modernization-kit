# Doxygen 由来の PlantUML 図の Mermaid 移行に関する検討

## 作業 WBS

- ✅ [タスク 0] リポジトリ内の手書き Markdown ドキュメント全 57 箇所の Mermaid 化と画像検証
- ⏳ [タスク 1] 手動作成 Doxygen コメント (`memory_lock.h`) の `@mermaid` 化
- ⏳ [タスク 2] `framework/doxyfw/templates/preprocess.sh` への `<mermaid>` タグ変換追加
- ⏳ [タスク 3] `framework/doxyfw/Doxyfile` の Mermaid 設定 (`MERMAID_PATH`, `MERMAID_RENDER_MODE`) 追加
- ⏳ [タスク 4] `framework/doxyfw/templates/extract-graphs.py` の Mermaid 出力対応 (`flowchart TD` / `flowchart LR`)
- ⏳ [タスク 5] `framework/doxyfw/docs/extract-graphs.md` の生成構文例示節の Mermaid 更新
- ⏳ [タスク 6] 各 app での `make doxy` 再生成と `doxybook2_*` 生成物の確認
- ⏳ [タスク 7] `text_style_jp.py` による Markdown スタイル検証

---

## 完了した作業 (手書き Markdown の Mermaid 化)

リポジトリ内の手書き管理 Markdown ドキュメント (合計 57 箇所) について、すべて PlantUML から同等の Mermaid 図への置換、画像レンダリング比較による意味の同一性確認、キャプション規則 (`CodeBlock: <caption-text>`) の適用、および `text_style_jp.py` の検証を完了しました。

### 完了ファイル一覧

1. `README.md` (1 箇所)
2. `app/c-modernization-kit/docs/github-actions.md` (2 箇所)
3. `app/general/docs/skill-guide/01-modern-development/about-modern-development.md` (2 箇所)
4. `app/general/docs/skill-guide/README.md` (1 箇所)
5. `app/service-sample/docs/service-events.md` (2 箇所)
6. `app/service-sample/prod/src/cmd/service-sample/README.md` (3 箇所)
7. `framework/makefw/docs/template-auto-selection.md` (1 箇所)
8. `framework/testfw/docs/about-shared-lib-static-linking.md` (1 箇所)
9. `app/cplat/docs/memory-lock.md` (2 箇所)
10. `app/cplat/prod/libsrc/cplat/console/README.md` (1 箇所)
11. `app/cplat/prod/libsrc/cplat/runtime/README.md` (1 箇所)
12. `app/cplat/docs/prompts/current/cplat-account-session-discussion.md` (1 箇所)
13. `app/porter/docs/architecture.md` (6 箇所)
14. `app/porter/docs/config.md` (5 箇所)
15. `app/porter/docs/sequence-bidir.md` (4 箇所)
16. `app/porter/docs/sequence-health.md` (2 箇所)
17. `app/porter/docs/sequence-lifecycle.md` (6 箇所)
18. `app/porter/docs/sequence-tcp.md` (9 箇所)
19. `app/porter/docs/sequence-data.md` (9 箇所)
20. `framework/doxyfw/docs/extract-graphs.md` (1 箇所: L17 パイプライン図)
21. `framework/doxyfw/docs/commands.md` (1 箇所: L944 判断フローチャート)
22. `framework/docsfw/docs/browser-reuse.md` (2 箇所: 共有ブラウザー処理フロー、コンポーネント接続図)
23. `framework/docsfw/docs/chrome-wrapper.md` (1 箇所: 処理フロー)
24. `framework/docsfw/docs/rsvg-convert.md` (1 箇所: 処理フロー)
25. `framework/doxyfw/docs/doxybook2-decolorize-output-spec.md` (1 箇所: 処理フロー)
26. `framework/doxyfw/docs/doxygen-colorize-output-spec.md` (1 箇所: 処理フロー)
27. `app/doxygen-sample/prod/src/markdown_sample.md` (1 箇所: シーケンス図サンプル)

---

## 次のステップ (機械的な対処が必要な場所)

### ステップ 1: 手動作成 C ソース コメントの @mermaid 化

- **対象**: `app/cplat/prod/include/cplat/runtime/memory_lock.h`
    - L130-153: `@startuml cplat_memory_lock_self のロック取得`
    - L175-204: `@startuml cplat_memory_lock_scope_release のロック解放`
- **作業内容**:
    - Doxygen 1.17.0 のネイティブ コマンド `@mermaid` 〜 `@endmermaid` を使用して Mermaid シーケンス図コードに書き換える。
    - Doxygen により XML 出力で `<mermaid>...</mermaid>` が生成される。

### ステップ 2: preprocess.sh への <mermaid> タグ変換追加

- **対象**: `framework/doxyfw/templates/preprocess.sh`
- **作業内容**:
    - Doxygen XML 内の `<mermaid>` タグを、Doxybook2 が解釈可能な Markdown の ````mermaid` コード フェンスへ変換する sed ルールを追加する。

  ```bash
  sed -e 's|\s*<mermaid\([^>]*\)>|\n\n```mermaid\n|g' \
        -e 's|</mermaid>|\n```\n|g'
  ```

### ステップ 3: framework/doxyfw/Doxyfile への Mermaid 設定追加

- **対象**: `framework/doxyfw/Doxyfile`
- **作業内容**:
    - `MERMAID_PATH = $(MERMAID_HOME)` (または devbin-win パス)
    - `MERMAID_RENDER_MODE = CLI` (または AUTO)
  を設定し、Doxygen 実行時に Mermaid 構文のパースと XML 出力を確実に有効化する。

### ステップ 4: extract-graphs.py の Mermaid 出力対応

- **対象**: `framework/doxyfw/templates/extract-graphs.py` (1,334 行)
- **作業内容**:
    - Doxygen XML からグラフ要素 (インクルード依存 `incdepgraph`、被インクルード `invincdepgraph`、継承関係 `inheritancegraph`、コラボレーション図 `collaborationgraph`、コールグラフ) を抽出し、従来の PlantUML 構文ではなく Mermaid 構文 (`flowchart TD` / `flowchart LR`) を出力する。
    - XML 挿入時のタグを `<mermaid>` タグに変更する。
    - ノードの強調表示 (対象ノードのスタイル定義 `:::selfNode` および `classDef selfNode fill:#ADD8E6,stroke:#333`) を Mermaid 形式に対応させる。
    - Mermaid のキャプション規則 `CodeBlock: <caption-text>` をコード ブロック直後に付与する。

### ステップ 5: extract-graphs.md の構文例示節の更新

- **対象**: `framework/doxyfw/docs/extract-graphs.md` (L139-184)
- **作業内容**:
    - 「生成される PlantUML の例」節を、ステップ 4 で出力される「生成される Mermaid の例」に更新し、ドキュメントの整合性を確保する。

### ステップ 6: make doxy 再生成と doxybook2_* 生成物の確認

- **作業内容**:
    - `app/cplat`、`app/calc`、`app/porter` 等で `make doxy` を実行し、生成された `doxybook2_*` 内のグラフが Mermaid コード ブロックとして正常に出力されていることを確認する。
