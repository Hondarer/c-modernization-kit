<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: claude code / セッション ID: session_01KJc53SRADvaQqA3NfkaSTv
上流文書: `261112-ci-parallelism-evaluation-discussion.md` (検討用 Markdown)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。セッション情報が見つからない場合も異常とせず、文書の記載から継続してください。
-->

# CI の並列度の評価と経過時間の表示 最終仕様

## 目的

CI の並列度の引き上げ (`MAKEFW_CPU_BUDGET` を 8 に指定) の評価を確定します。  
あわせて、ビルドとテストの経過時間を、app ごと、および app の中の段階ごとにログへ表示します。時間のかかる箇所を、ログから直接読み取れるようにするためです。

## 残課題ごとの扱い

対処元は、`261111-ci-time-backlog-followup-backlog.md` の BL-001 から BL-006 です。

| 課題 | 扱い |
| --- | --- |
| BL-001 並列度の引き上げの効果の評価 | Linux は「効果あり」として閉じる。Windows は、差を実行ごとのばらつきとして扱い、閉じる。`ci.yml` は変更しない |
| BL-002 Windows の Run tests ステップの時間 | ばらつきとして閉じる |
| BL-003 app の中の時間の内訳 | この仕様で対処する |
| BL-004 コンパイル結果のキャッシュとジョブの分割 | 行わずに閉じる |
| BL-005 `make docs` の時間の短縮 | 見送り、新しい残課題一覧に引き継ぐ |
| BL-006 doxyfw のテストの失敗 (実行 42978073210) | 原因不明として閉じる |

Table: 残課題ごとの扱い

> [!NOTE]
> 並列度を上げた後の Build ステップの中央値は、OL8 が 120[分] (上げる前は 153[分])、OL9 が 89[分] (同 145[分])、OL10 が 67[分] (同 104[分])、Windows が 138[分] (同 149[分]) です。  
> Windows の Run tests ステップは、上げた後の 3 回が 118 ～ 137[分] で、上げる前の過去 5 回 (69 ～ 103[分]) より長い値でした。並列度 4 はランナーの論理 CPU 数を超えず、MSVC の `/MP` も 1 のままであるため、設定の上で遅くなる仕組みは確認していません。この差は、ばらつきとして扱います。

## 経過時間の表示

### app ごとの表示

app を依存の順に実行するスクリプト (`run_ordered_subdir_target.sh`) の終了の行に、経過時間を加えます。

```text
INFO: done [3/17] /path/to/app/cplat rc=0 elapsed=49:12
```

- 対象は、`done` と `interrupted` の行です。開始 (`dispatch`) から終了までの時間を表示します。
- 書式は、既存の待機の表示 (`waiting elapsed=`) と同じです。1 時間未満は `MM:SS`、1 時間以上は `HH:MM:SS` です。
- 開始または終了の時刻を取得できなかった場合 (`now_seconds` が 0 を返した場合) は、`elapsed=` を表示しません。
- このスクリプトは、app の中の走査 (ライブラリやテストのフォルダーの並列実行) にも使われます。その場合も同じ形式で表示します。

### app の中の段階ごとの表示

app 直下の `makefile` (テンプレート `__each_app_template.mk`) が子の `make` を実行した後に、経過時間を 1 行表示します。

```text
make -C prod
...
INFO: cplat prod elapsed=00:18
make -C test
...
INFO: cplat test elapsed=48:40
```

| ゴール | 表示する段階 | 表示の例 |
| --- | --- | --- |
| 既定 (`make`) | `prod` のビルド | `INFO: cplat prod elapsed=00:18` |
| 既定 (`make`) | `test` のビルド | `INFO: cplat test elapsed=48:40` |
| `test` | テストの実行 (`make -C test _test_run`) | `INFO: cplat test _test_run elapsed=31:05` |

Table: 段階ごとの表示

- `make test` は、最初に既定のゴールを実行します。この部分は、既定のゴールの表示 (`prod`、`test`) がそのまま出ます。
- 子の `make` が失敗した場合も、失敗した段階の経過時間を表示します。
- 子の `make` を実行しなかった場合 (署名が一致してビルドやテストを省いた場合、`makefile` がないディレクトリ) は、表示しません。
- `gen`、`clean`、`doxy`、`with-cov` のゴールには表示を加えません。
- 表示は、app 直下で `make` を実行した場合にも出ます。

> [!NOTE]
> コンパイルとリンクを分けた時間は表示しません。末端の makefile テンプレートの規則ごとに時刻を取る必要があり、変更が大きくなるためです。

### 実装の条件

Windows では、プロセスを 1 つ起動するたびに時間がかかります。このため、時間の表示のためにプロセスを増やしません。

- テンプレートのレシピでは、bash の変数 `SECONDS` (シェルの起動からの秒数) の差で経過時間を求め、組み込みの `printf` で表示します。`date` などの外部コマンドや、コマンド置換 (`$(...)`) を使いません。
- レシピのシェルが `SECONDS` を持たない場合 (値が数値でない場合) は、表示を省きます。ビルドの結果には影響させません。
- スクリプトでは、開始と終了の時点ですでに取得している時刻 (`now_seconds` の戻り値) を使います。`now_seconds` の呼び出しを増やしません。

## 変更対象

| リポジトリ | ファイル | 変更 |
| --- | --- | --- |
| `framework/makefw` | `bin_internal/run_ordered_subdir_target.sh` | `done` と `interrupted` の行に `elapsed=` を追加 |
| `framework/makefw` | `makefiles/__each_app_template.mk` | 既定のゴールと `test` のゴールに、段階ごとの表示を追加 |
| `framework/makefw` | `bin_test/` | 表示を検証するテストを追加。既存のテストが `done` の行の全体を照合している場合は、あわせて直す |
| `framework/makefw` | `docs/build-configurations.md` | 経過時間の表示の説明を追加 |
| app のサブモジュール 7 個 (cjson、cplat、lua、openssl、porter、sqlite、zlib) | `makefile` | `update_template_makefiles.py` でテンプレートに同期 |
| 親リポジトリ | `app/<name>/makefile` 10 個 (calc、calc.net、doxygen-sample、empty-lib、override-sample、service-sample、string-catalog-sample、struct-meta、subfolder-sample、tutorial) | 同上 |
| 親リポジトリ | サブモジュールの参照 (makefw と 7 個の app) | 更新 |

Table: 変更対象

`.github/workflows/ci.yml` と `.jenkins/` は変更しません。

app の `makefile` は、テンプレートの複製です。手で編集せず、`python framework/makefw/bin/update_template_makefiles.py` で同期します。

## 知見の反映計画

| 反映先 | 内容 |
| --- | --- |
| `framework/makefw/docs/build-configurations.md` | 経過時間の表示の形式と、表示される条件 |
| `__each_app_template.mk` のコメント | 外部コマンドを使わずに `SECONDS` で求める理由 (Windows のプロセスの起動の時間) |

Table: 知見の反映先

## 受入条件

1. 表示を検証するテストを `framework/makefw/bin_test/` に追加し、手元の Windows で makefw の `bin_test/` が成功する。手元の環境の不足によるスキップは、`--allow-skips` で分けて報告する。
2. 手元の Windows で、ビルド済みの app の `make` (何もビルドしない実行) の所要時間を、変更の前後で 5 回ずつ測る。変更後の中央値が、変更前の 5 回の範囲 (最小値から最大値) の中にある。
3. テンプレートとスクリプトの変更で増えるプロセスの起動が 0 個であることを、差分で確認する。
4. `update_template_makefiles.py --dry-run` が、同期の後に対象なしを報告する。
5. 変更した Markdown は、`text_style_jp.py --dry-run` で指摘がない。
6. GitHub Actions の実走 1 回で、全ジョブが成功する。
7. その実走の Linux と Windows のログに、`INFO: done ... elapsed=` の行と、`INFO: <app> prod elapsed=`、`INFO: <app> test elapsed=`、`INFO: <app> test _test_run elapsed=` の行がある。

受入条件 6 または 7 を満たさない場合は、原因を修正して再コミットし、もう一度実走で確認します。この案件の変更が原因でない失敗は、分けて報告します。

> [!IMPORTANT]
> 受入条件 2 のために、手元の Windows で app を 1 つ選び、`make` を合計 10 回実行します。対象は、ビルド済みで依存の少ない app (候補は `app/calc`) とし、ワークスペース全体のビルドとテストは実行しません。対象の app がビルド済みでない場合は、最初の 1 回がビルドになります。

## コミットと push

- 対象は、`framework/makefw`、7 個の app のサブモジュール、親リポジトリの `main` です。実走で問題が起きた場合の再コミットと、完了の記録のコミットを含みます。
- 順序は、makefw、app のサブモジュール、親リポジトリです。親リポジトリのコミットで CI を起動します。
- サブモジュールのコミットは、親リポジトリの CI を起動しません。メッセージに `[skip ci]` は付けません (各リポジトリが独自の CI を持つ場合に、その実行を止めないためです)。
- `app/general/docs/prompts/` 配下だけを変更するコミットには、`[skip ci]` を付けます。
- 別の作業の push と並行する場合は、push の前に取り込みます。別の作業の未コミットの変更は、コミットに含めません。

## 対象外

- `.github/workflows/ci.yml` の並列度の指定の変更
- コンパイルとリンクを分けた時間の表示
- コンパイル結果のキャッシュ、ジョブの分割
- `docs` ジョブの時間の短縮
