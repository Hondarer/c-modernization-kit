<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新日時: 2026-11-15 14:11:30 JST
最終更新エージェント: grok (セッション ID: 01a1dd8f-3288-7e31-9b7f-aba93a5e1ef7)
稼働 OS: Linux (WSL)
検討 ID: makefw-windows-backlog-20261114
上流文書: `261114-makefw-windows-backlog-design.md`、`261114-makefw-windows-backlog-discussion.md`。削除されている場合があります。
再開手順: agree-before-implement スキルを読み込み、存在する上流文書と作業 WBS を確認してください。この残課題を対策する場合は別案件の検討から開始します。過去の記載は削除せず、対処と経緯を更新してください。
-->

# makefw の残課題一覧

ファイル名: `261114-makefw-windows-backlog-backlog.md`

## 案件の識別

案件名: `makefw-windows-backlog-20261114`  
作成日: `2026-11-15`  
整理状態: 整理済み

## 残課題一覧

Table: 本案件で残す環境上の課題

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | Linux の旧 OpenSSL mock が現行 app の mock より優先される | 作業中摘出・既存環境 | 中 | 対策しない |

## BL-001 Linux の旧 OpenSSL mock が現行 app の mock より優先される

### 現状の事象と本来あるべき姿

WSL の通常のライブラリ探索順では、`framework/testfw/lib/linux_el8_x64/libmock_openssl.a` が、現在の生成先である `app/openssl/test/lib/libmock_openssl.a` より先に選ばれます。  
旧アーカイブには 15 メンバーが残っており、現行 app のソースは 82 個です。  
現行の cJSON・OpenSSL 共有ライブラリと mock を局所ビルドした後でも、`make -C app/porter/test/src/integration/certificateTest -j2 test` は旧アーカイブを使用し、`EVP_EncryptUpdate@@OPENSSL_1_1_0` の未定義参照と `DSO missing from command line` で失敗しました。

通常のビルドでも、現在の生成元に対応する mock を使用し、OpenSSL 3.5.9 の `_app` 付き共有ライブラリへ委譲できる状態が必要です。  
旧配置の生成物が残っていることで、現在の app に対するテスト結果が変わる状態を解消します。

### 摘出の契機と今回の見送り理由

本案件の Linux 摘出元検証で確認しました。  
makefw のリンク構成追跡・警告保存・ソース分類とは別に、旧配置の生成物の整理と移行手順を確認する必要があります。  
既存環境の旧アーカイブを直接削除したり、ワークスペース全体の探索順を変更したりする対処は実施していません。

本案件の局所検証では、コマンド ラインの `LIBSDIR` で現行 app の製品・テスト ライブラリを先に、testfw と Google Test のライブラリを後に指定して生成しました。  
依存生成物の再生成後、その同一バイナリの 31 件を Linux の一時領域から個別プロセスで実行し、全件成功しました。  
これは検証時だけの指定であり、通常の探索順の問題を解消したものではありません。

### 影響範囲と放置した場合のリスク

旧 `libmock_openssl.a` が残存する Linux ワークスペースで、`mock_openssl` をリンクするテストが影響を受けます。  
古い OpenSSL の直接参照や古い mock の実装が混入し、現行ライブラリを生成してもリンク失敗や誤ったテスト結果が発生するおそれがあります。  
新しいワークスペースで同じ旧アーカイブが生成されるかは未確認です。

### 推奨される恒久対応

旧アーカイブの生成元と現在の配置規則を確認し、生成物の移行・清掃手順で旧配置を整理する方針を検討します。  
必要な場合は生成元の clean または移行処理を修正し、現在必要な testfw のライブラリを削除しない条件を明示します。  
既存の旧アーカイブがある環境と、新規環境の両方で、追加の `LIBSDIR` 指定なしに `certificateTest` が現行 mock を使用して成功することを受入条件とします。

### 旧アーカイブは無く、恒久対策は行わない

2026-11-15 のユーザー判断で、探索順の変更と旧配置の清掃手順は行わず、この課題を対策しないこととしました。  
作業ツリーの `framework/testfw/lib/linux_el8_x64/libmock_openssl.a` は存在しません。  
現行の OpenSSL mock は `app/openssl/test/libsrc/mock_openssl` から `app/openssl/test/lib/libmock_openssl.a` へ生成されます。  
`framework/testfw/libsrc` に `mock_openssl` はなく、現在の構成が同じパスへ旧アーカイブを再生成する経路はありません。  
同じディレクトリにある `libmock_libssh.a` は、`framework/testfw/libsrc/mock_libssh` が生成する mock です。

参照先:

- [makesrc_c_cpp.mk](../../../../../framework/makefw/makefiles/makesrc_c_cpp.mk) の `LIBSDIR` と `LIBSFILES`
- [OpenSSL mock の出力先](../../../../openssl/test/libsrc/makepart.mk)
- [実委譲ライブラリ名](../../../../openssl/test/include/mock_openssl.h)
