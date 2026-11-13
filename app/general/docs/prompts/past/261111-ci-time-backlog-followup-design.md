<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: claude code / セッション ID: session_01KJc53RdMg7fob2AYQLvdRm
上流文書: `261111-ci-time-backlog-followup-discussion.md` (検討用 Markdown)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。セッション情報が見つからない場合も異常とせず、文書の記載から継続してください。
-->

# CI の並列度の引き上げと残課題への対応 最終仕様

## 目的

GitHub Actions の CI の待ち時間を短くします。あわせて、ドキュメント生成ジョブの分離で残った課題 (`../past/261111-ci-docs-job-split-backlog.md` の BL-001 から BL-004) を整理します。

## 背景となる事実

- CI のランナーは 4 論理 CPU です。makefw は CPU 予算の半分 (`ceil(CPU 数 / 2)`) をコンパイルに使うため、CI は `make -j2` で動いています。
- 実行 42930377100 では、Build ステップが 79 ～ 145[分]、Run tests ステップが 33 ～ 76[分] でした。大半は app/cplat のテストのビルドと実行です。
- 同じ内容でも、実行ごとに Build ステップの時間が変わります。過去 5 回の中央値は、OL8 が 153[分]、OL9 が 145[分]、OL10 が 104[分]、Windows が 149[分] です。
- CI は、`main` への push のたびに全ジョブを実行します。Markdown だけのコミットでも同じです。

## 対応の範囲

| 課題 | 対応 |
| --- | --- |
| BL-001 テストのビルドと実行の時間の短縮 | CI の CPU 予算を指定し、並列度を 2 から 4 に上げる。あわせて、記録だけのコミットで CI を起動しない運用を定める |
| BL-002 `make docs` の時間の短縮 | 対処しない。残課題一覧へ引き継ぐ |
| BL-003 生成漏れの照合の対象範囲 | 生成ソースの置き場所の規則を文書に記載する。照合の仕組みは変えない |
| BL-004 `cc` がない PC での `test_ident_batch` の失敗 | `cc` が見つからない場合は、理由を表示してスキップする |

Table: 課題ごとの対応

コンパイル結果のキャッシュ (ccache) とジョブの分割は、今回は扱いません。今回の測定結果とあわせて、残課題一覧に記載します。

## 変更対象

### .github/workflows/ci.yml

`build-and-test-linux` と `build-and-test-windows` の、Build と Run tests の 2 つのステップに、環境変数 `MAKEFW_CPU_BUDGET` を指定します。値は 8 です。

```yaml
      - name: Build
        env:
          # ランナーは 4 論理 CPU でビルド専用のため、CPU をすべて使う。
          # makefw は CPU 予算の半分をコンパイルに使うので、予算を論理 CPU 数の 2 倍に指定する。
          # see: framework/makefw/docs/build-configurations.md (並列ビルド)
          MAKEFW_CPU_BUDGET: "8"
        run: |
          ...
```

この指定により、makefw の式は次の値を算出します。makefw 本体は変更しません。

| OS | make | MSVC の `/MP` | テストの同時実行数 |
| --- | ---: | ---: | ---: |
| Linux | `-j4` (変更前は `-j2`) | - | 4 (変更前は 2) |
| Windows | `-j4` (変更前は `-j2`) | `/MP1` (変更なし) | 4 (変更前は 2) |

Table: MAKEFW_CPU_BUDGET を 8 にした場合の並列度

> [!NOTE]
> 環境変数をジョブ全体ではなくステップに指定するのは、`make bin-test` (Run script tests ステップ) が実行する makefw のテストへ値が伝わらないようにするためです。`docs` ジョブには指定しません。

> [!IMPORTANT]
> テストの同時実行数は、`MAKEFW_TEST_RUN_JOBS` の指定がなければ make の並列度と同じ値になります。今回は指定せず、4 にします。

実走で問題が起きた場合は、次のとおり対処して再コミットします。

- メモリ不足 (コンパイラやリンカーの異常終了、MSVC の C1060 の多発) が起きた OS は、`MAKEFW_CPU_BUDGET` の指定を外します。
- タイムアウトなど、テストのタイミングが原因の失敗が起きた場合は、Run tests ステップに `MAKEFW_TEST_RUN_JOBS: "2"` を追加します。

### app/general/docs/workspace-agent-workflow.md

「Git の扱い」に、次の内容を記載します。

- `app/general/docs/prompts/` 配下だけを変更するコミットは、コミット メッセージに `[skip ci]` を含める。ビルド、テスト、公開する成果物に影響せず、CI の 1 回の実行に数時間かかるためである。
- GitHub Actions は、push に含まれる最新のコミットのメッセージで判定する。ほかの変更を含むコミットと一緒に push する場合は、CI が起動する。
- `[skip ci]` を付けたコミットの内容は、次に CI が動いた時点で GitHub Pages に反映される。
- 根拠の URL ([Skipping workflow runs](https://docs.github.com/en/actions/managing-workflow-runs-and-deployments/managing-workflow-runs/skipping-workflow-runs))。

この案件の記録だけのコミットにも、同じ運用を適用します。

### framework/makefw/docs/build-configurations.md

「生成ソースだけを作る (make gen)」に、次の内容を記載します。

- 生成ソースは、生成物の置き場所 `$(GENDIR)` (`gen/`) に置く。makefw の生成規則 (flex、bison、`.mc`) はこの場所へ出力する。app が独自の規則で生成する場合も、同じ場所へ出力する。
- 理由は、`make gen` の対象と、Doxygen の入力を、`gen/` ディレクトリで判別するためである。

> [!NOTE]
> c-modernization-kit の CI の `verify-gen` ジョブが `gen/` ディレクトリだけを照合している点は、makefw の文書には記載しません。makefw は、利用するプロジェクトの CI の構成を前提にしないためです。

### framework/makefw/bin_test/test_ident_batch.py

`cc` が `PATH` に見つからない場合は、2 件のテストをスキップします。理由は `cc is not found` とします。テストが検証する内容は変えません。

`make bin-test` は、既存の規範どおり、このスキップを想定外のスキップ (失敗) として扱います。`BIN_TEST_ARGS=--allow-skips` を指定した場合は、失敗にしません。CI には `cc` があるため、テストは引き続き実行されます。

### 対処元の残課題一覧

`../past/261111-ci-docs-job-split-backlog.md` の BL-001 から BL-004 に、今回の対処と経緯を追記します。項目とファイルは削除しません。

## 知見の反映計画

| 反映先 | 内容 |
| --- | --- |
| `.github/workflows/ci.yml` のコメント | CPU 予算を論理 CPU 数の 2 倍に指定する理由 |
| `app/general/docs/workspace-agent-workflow.md` | 記録だけのコミットに `[skip ci]` を付ける運用 |
| `framework/makefw/docs/build-configurations.md` | 生成ソースを `gen/` に置く規則 |

Table: 知見の反映先

## 受入条件

1. `test_ident_batch.py` は、`cc` がない Windows PC で 2 件がスキップになり、エラーにならない。
2. 変更した Markdown は、`text_style_jp.py --dry-run` で指摘がない。
3. GitHub Actions の実走で、全ジョブが成功する。
4. 実走のログで、Linux と Windows の Build ステップが `make -j4` で動いている。
5. 実走で、4 つのジョブ (OL8、OL9、OL10、Windows) すべての Build ステップが、過去 5 回の中央値 (153[分]、145[分]、104[分]、149[分]) より短い。
6. 実走で、テストのタイムアウトによる失敗がない。
7. 実走の `make bin-test` で、`test_ident_batch` がスキップされずに成功する。

結果の報告には、Build ステップと Run tests ステップの時間を分けて記載し、過去 5 回の最小値 (121[分]、118[分]、79[分]、109[分]) との比較も併記します。

受入条件 3 から 7 を満たさない場合は、「変更対象」に記載した対処を行って再コミットし、もう一度実走で確認します。

## 対象外

- makefw の並列度の算出式 (`_parallel.mk`) の変更
- コンパイル結果のキャッシュ、ジョブの分割
- `docs` ジョブの時間の短縮 (BL-002)
- `.jenkins/` の構成
