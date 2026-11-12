<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新エージェント: claude code / セッション ID: session_01KJc53RRnu4roDTnMUeKvMT
下流文書: `261111-third-party-lib-rename-design.md` (最終仕様)、`261111-third-party-lib-rename-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# 第三者管理ライブラリの名前変更と OpenSSL の共有ライブラリ化の検討

ファイル名: `261111-third-party-lib-rename-discussion.md`

## 検討の識別

検討 ID: third-party-lib-rename

版: 4

状態: 合意済み

## 作業 WBS と進捗管理

この検討用 Markdown が管理する作業 WBS です。  
項目 4 の作業項目は現時点の見込みであり、回答に応じて見直します。  
見送りの決定や範囲外の気付きは、生じた時点で `261111-third-party-lib-rename-backlog.md` に追記します。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 現状の調査 (app/openssl の静的リンク構成、zlib 方式の mock、各共有ライブラリの SONAME、OpenSSL の `shlib_variant`)
    - ✅ 1.2. 確認項目 ([Q-001](#q-001-対象とする第三者管理ライブラリ) から [Q-010](#q-010-受入検証の範囲)) の作成
    - ✅ 1.6. 最終仕様の差し戻しの反映 ([Q-011](#q-011-openssl-の-mock-の呼び出し元情報) の記録と、ヘッダー差し替え方式の Windows での追加検証)
    - ✅ 1.3. ユーザー回答の受付と記録 ([Q-001](#q-001-対象とする第三者管理ライブラリ) から [Q-010](#q-010-受入検証の範囲))
    - ✅ 1.4. 回答に基づく前提・制約の検証と WBS の見直し
    - ✅ 1.5. 事前検証 (Linux と Windows の一時フォルダーで、`shlib_variant` に `_app` を指定した OpenSSL の共有ビルドと、mock だけをリンクする実委譲を試す)
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `261111-third-party-lib-rename-design.md` の作成 (差し戻しを受けて「OpenSSL の mock」を改訂)
    - ✅ 3.2. 最終仕様における受入条件と整合性の確認
    - ✅ 3.3. 対話セッションにおける最終仕様の承認の受付 (第 2 ゲート) (2026-11-12 承認)
- 🔄 4. 承認された最終仕様に従った作業の実施
    - ⚠️ 4.1. 第 1 段階: makefw 製の第三者管理ライブラリの名前変更 (`app/zlib`、`app/cjson`、`app/lua`、`app/sqlite` の `prod/libsrc/*/makepart.mk`、README.md、AGENTS.md)
    - ✅ 4.2. 第 1 段階: 各 mock の実委譲先ライブラリ名と公開シンボル検査の更新 (`test/include/mock_*.h`、`test/src/lib*Test/exportTest/`)
    - 🔄 4.3. 第 1 段階: 利用側のリンク指定と同梱定義の更新 (`app/cplat`、`app/porter`、`app/struct-meta`、`app/lua`、`app/sqlite` などの `makepart.mk`、`app/cplat/prod/runtime-bundle.mk`、`app/cplat/docs/link-policy.md`) と局所テスト
    - ✅ 4.4. 第 2 段階: OpenSSL の共有ライブラリ化 (`app/openssl/bin_internal/build_openssl.py`、`bin_test/test_build_openssl.py`、README.md、AGENTS.md)
    - ✅ 4.5. 第 2 段階: OpenSSL の mock の変更 (`app/openssl/test/include/mock_openssl.h`、`test/include_override/openssl/`、`test/libsrc/mock_openssl/`。呼び出し元情報の 3 引数を維持し、実委譲先を共有ライブラリに変え、置き換える関数を 17 個に広げる)
    - ✅ 4.6. 第 2 段階: cplat の OpenSSL リンク方法の変更 (`app/cplat/prod/libsrc/cplat/makepart.mk`、`exports.map`、`prod/runtime-bundle.mk`、`docs/link-policy.md`、crypto 関連テストの `makepart.mk`) と局所テスト
    - ✅ 4.7. 正本ドキュメントへの命名規則の反映 (`app/general/docs/shared-library-mock-guideline.md`、`app/general/docs/vscode-variables.md` などの該当節)
    - ✅ 4.8. 局所テストと `text_style_jp.py` による確認
    - ⚠️ 4.9. 受入条件への適合確認 (未確認の事項: Windows のビルドとテスト。確認方法: CI。CI に必要なコミットとプッシュはユーザーの指示を受けてから行う)
    - ✅ 4.10. 残課題一覧 (`261111-third-party-lib-rename-backlog.md`) の再構築と整理
- 🚫 5. 成果物の承認の受付 (第 3 ゲート)
    - ✅ 5.1. 成果物・検証結果・残課題の提示
    - 🚫 5.2. 対話セッションにおける成果物の承認の受付 (停止理由: ユーザーの承認待ち) ← **CURRENT**

## 対象と参照資料

対象成果物: 承認済み最終仕様の「変更対象」に記載した各 app のビルド設定、mock、文書

今回の対象範囲: zlib、cjson、lua、sqlite の名前変更と利用側の更新、OpenSSL の共有ライブラリ化と mock の実委譲

書き込み可能な検討用 Markdown: この文書 (`app/general/docs/prompts/current/261111-third-party-lib-rename-discussion.md`)

> [!NOTE]
> 依頼では出力先が `app/general/docs/current` と指定されました。  
> このディレクトリは存在せず、既存の検討用 Markdown は `app/general/docs/prompts/current/` にあるため、こちらへ配置しました。

参照資料と版:

- `app/openssl/README.md`、`app/openssl/AGENTS.md`、`app/openssl/bin_internal/build_openssl.py` (コミット 5e6f9c08 時点のワークスペース)
- `app/zlib/README.md`、`app/zlib/AGENTS.md`、各 app の `test/include/mock_*.h`
- `app/general/docs/shared-library-mock-guideline.md`
- `framework/testfw/docs/about-shared-lib-static-linking.md`
- OpenSSL 3.5.9 の `Configurations/README.md` (`shlib_variant` の節)、`Configurations/shared-info.pl`、`util/libcrypto.num`、`util/libssl.num`

## 調査で確認した事実

### 現在のライブラリ名

| app | Linux の成果物 | 種別 | システム側の同名ファイル (この開発環境の `/lib64`) |
| --- | --- | --- | --- |
| zlib | `libzlib.so` | 共有 | なし (システム側は `libz.so`) |
| cjson | `libcjson.so` | 共有 | 開発用パッケージを導入すると `libcjson.so` が置かれる |
| lua | `liblua.so` | 共有 | 開発用パッケージを導入すると `liblua.so` が置かれる (現在は `liblua-5.3.so` のみ) |
| sqlite | `libsqlite3.so` | 共有 | 開発用パッケージを導入すると `libsqlite3.so` が置かれる (現在は `libsqlite3.so.0` のみ) |
| openssl | `libcrypto.a`、`libssl.a` | 静的 | `libcrypto.so`、`libssl.so` (1.1.1) がある |

Table: 第三者管理ライブラリの現在の成果物名

makefw が作る共有ライブラリには SONAME がありません。  
SONAME は、共有ライブラリ自身に記録される名前であり、利用側の実行ファイルはこの名前で実行時にライブラリを探します。  
SONAME がない場合は、リンク時のファイル名 (`libcjson.so` など) がそのまま記録されます。

makefw 製のライブラリ名は、`prod/libsrc/` 直下のディレクトリ名から決まります (`prod/libsrc/zlib/` が `libzlib.so` になります)。

### ファイル名の変更で解決する問題と残る問題

ファイル名の変更で解決する問題は次のとおりです。

- リンク時に、`LIBS += crypto` がシステムの `libcrypto.so` へ解決される問題
- 実行時に、ローダーが同名のシステム側ライブラリを読み込む問題
- Windows で、`PATH` 上の別製品 (Git、Python、Strawberry Perl) が同梱する `libcrypto-3-x64.dll` を読み込む問題

ファイル名の変更だけでは残る問題は、Linux のシンボル名の衝突です。  
Linux では、ライブラリ名が違っても関数名 (`EVP_EncryptInit_ex` など) はプロセス全体で 1 つの名前空間に入ります。  
システムの libcrypto を直接リンクした実行ファイルが libcplat.so を読み込むと、libcplat からの呼び出しがシステム側の関数へ束縛されることがあります。  
現在の構成は、静的リンクと `--exclude-libs` で OpenSSL を libcplat.so の内部に隠し、この問題を避けています。

### OpenSSL の shlib_variant

OpenSSL のビルド システムには、システムの OpenSSL と共存させるための設定 `shlib_variant` があります。  
`Configurations/README.md` によると、`shlib_variant => "-abc"` を指定すると次のようになります。

- 共有ライブラリのファイル名と SONAME が `libssl-abc.so.<バージョン>` の形になります。
- シンボル バージョン (各関数に付く版の名前) が、`OPENSSL_<バージョン>` から `OPENSSL_ABC_<バージョン>` に変わります。

シンボル バージョンが変わると、このライブラリをリンクした利用側の呼び出しと、システムの OpenSSL をリンクした側の呼び出しが、互いに相手のライブラリへ束縛されなくなります。  
`platform/Windows/MSVC.pm` も同じ設定を DLL 名に反映します。  
指定はターゲット定義ファイルを追加して Configure に渡す方法で行え、OpenSSL 本体へのパッチは不要です。

この設定を使った実際のビルドは、まだ行っていません。

### OpenSSL の公開関数の数

`util/libcrypto.num` は 5,926 行、`util/libssl.num` は 608 行です。  
zlib の mock は公開関数をすべて API 表に載せていますが、OpenSSL で同じことを行うと、約 6,500 個の `MOCK_METHOD` を持つ mock になります。

## 現行の合意内容

ユーザーが 2026-11-11 の対話で明示した内容は次のとおりです。

- 目的は、app/openssl を共有ライブラリにして、mock から実関数へ委譲できるようにすることです。
- 手段として、第三者管理ライブラリの名前を、システムのライブラリと重複しない名前に変えます (例: `libapp_openssl`)。
- 第三者管理ライブラリは、すべて同じ規則で名前を変えます (zlib、cjson、lua など)。
- 第一者管理ライブラリの名前は変えません。

解決済みの Q で確定した内容は次のとおりです。

- 対象は zlib、cjson、lua、sqlite、openssl の 5 つの app です ([Q-001](#q-001-対象とする第三者管理ライブラリ))。
- 新しい名前は、全ライブラリで接尾辞 `_app` を付けた形にそろえます ([Q-003](#q-003-名前の形式))。
- OpenSSL は `shlib_variant` を使い、ファイル名、SONAME、シンボル バージョンを変えます ([Q-002](#q-002-openssl-のシンボル衝突対策))。
- mock ライブラリの名前は変えません ([Q-004](#q-004-mock-ライブラリの名前))。
- OpenSSL の mock は、app が利用している関数だけを載せた部分的な API 表とします。利用側は mock だけをリンクし、実関数へは mock から委譲します。必要な関数が mock にない場合は、その都度追加します ([Q-005](#q-005-openssl-の-mock-の方式))。
- libcplat.so の製品ビルドも OpenSSL を動的リンクに変え、app/openssl は共有ライブラリだけを作ります ([Q-006](#q-006-cplat-製品ビルドの-openssl-リンク方法))。
- zlib、cjson、lua、sqlite のシンボル名の衝突への対策は対象外とし、残課題一覧に記録します ([Q-007](#q-007-makefw-製ライブラリのシンボル衝突対策の扱い))。
- 最終仕様の前に、Linux と Windows の一時フォルダーで事前検証を行います ([Q-008](#q-008-第-1-ゲート前の事前検証))。
- 作業は、makefw 製の 4 つの app の名前変更、OpenSSL の共有ライブラリ化の順に 2 段階で行います ([Q-009](#q-009-移行の進め方))。
- 受入検証は、Linux で対象 app と利用側 app のビルドとテストを実行し、Windows は CI で確認します ([Q-010](#q-010-受入検証の範囲))。
- OpenSSL の mock は、呼び出し元のファイル名、行番号、関数名を受け取る先頭 3 引数を維持します。ほかの第三者管理ライブラリの mock にこの機能がないことは、残課題一覧に記録します ([Q-011](#q-011-openssl-の-mock-の呼び出し元情報))。

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | 「現行の合意内容」に記載したユーザーの明示 |
| 範囲・境界 | 確認済み | [Q-001](#q-001-対象とする第三者管理ライブラリ)、[Q-004](#q-004-mock-ライブラリの名前)、[Q-007](#q-007-makefw-製ライブラリのシンボル衝突対策の扱い)、[Q-009](#q-009-移行の進め方) |
| 入力・根拠 | 確認済み | [事前検証の結果](#事前検証の結果) |
| 整合性・依存関係 | 確認済み | [Q-002](#q-002-openssl-のシンボル衝突対策)、[Q-003](#q-003-名前の形式)、[Q-005](#q-005-openssl-の-mock-の方式) (現行規範「実ライブラリと mock を同時リンクしない」と整合)、[Q-011](#q-011-openssl-の-mock-の呼び出し元情報) |
| 実現性・例外 | 確認済み | [事前検証の結果](#事前検証の結果)、[Q-005](#q-005-openssl-の-mock-の方式)、[Q-006](#q-006-cplat-製品ビルドの-openssl-リンク方法) |
| 受入条件・検証 | 確認済み | [Q-010](#q-010-受入検証の範囲) |

Table: 観点別の確認状況

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-対象とする第三者管理ライブラリ) | 対象とする第三者管理ライブラリ | 解決済み | なし |
| [Q-002](#q-002-openssl-のシンボル衝突対策) | OpenSSL のシンボル衝突対策 | 解決済み | [Q-003](#q-003-名前の形式) |
| [Q-003](#q-003-名前の形式) | 名前の形式 | 解決済み | [Q-002](#q-002-openssl-のシンボル衝突対策) |
| [Q-004](#q-004-mock-ライブラリの名前) | mock ライブラリの名前 | 解決済み | [Q-003](#q-003-名前の形式) |
| [Q-005](#q-005-openssl-の-mock-の方式) | OpenSSL の mock の方式 | 解決済み | なし |
| [Q-006](#q-006-cplat-製品ビルドの-openssl-リンク方法) | cplat 製品ビルドの OpenSSL リンク方法 | 解決済み | [Q-002](#q-002-openssl-のシンボル衝突対策) |
| [Q-007](#q-007-makefw-製ライブラリのシンボル衝突対策の扱い) | makefw 製ライブラリのシンボル衝突対策の扱い | 解決済み | [Q-002](#q-002-openssl-のシンボル衝突対策) |
| [Q-008](#q-008-第-1-ゲート前の事前検証) | 第 1 ゲート前の事前検証 | 解決済み | [Q-002](#q-002-openssl-のシンボル衝突対策) |
| [Q-009](#q-009-移行の進め方) | 移行の進め方 | 解決済み | [Q-001](#q-001-対象とする第三者管理ライブラリ) |
| [Q-010](#q-010-受入検証の範囲) | 受入検証の範囲 | 解決済み | [Q-009](#q-009-移行の進め方) |
| [Q-011](#q-011-openssl-の-mock-の呼び出し元情報) | OpenSSL の mock の呼び出し元情報 | 解決済み | [Q-005](#q-005-openssl-の-mock-の方式) |

Table: 質問一覧

## 明確化のための確認項目

### Q-001 対象とする第三者管理ライブラリ

**Status**

解決済み

**Question**

名前を変える対象は、zlib、cjson、lua、sqlite、openssl の 5 つの app でよいですか。

**Context**

依頼では「zlib, cjson, lua など」と例示されました。  
ワークスペースで外部の配布アーカイブを取り込んでいる app は、この 5 つです。  
sqlite は例示に含まれていませんが、`libsqlite3.so` はシステムの開発用パッケージが置くファイル名と同じです。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) zlib、cjson、lua、sqlite、openssl の 5 つを対象とする。
- [ ] B: zlib、cjson、lua、openssl の 4 つを対象とし、sqlite は対象外とする。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-12 06:47:27 JST (検討用 Markdown の保存時刻)

**Assessment**

5 つの app を対象とすることが確定しました。

**Recommendation**

推奨: A  
理由: 「第三者管理はすべて同じ規則」という前提に合い、sqlite だけが旧名で残る例外を作りません。

### Q-002 OpenSSL のシンボル衝突対策

**Status**

解決済み

**Question**

OpenSSL の共有ライブラリ化では、ファイル名の変更に加えて、`shlib_variant` によるシンボル バージョンの変更も行いますか。

**Context**

「調査で確認した事実」のとおり、ファイル名を変えても Linux のシンボル名の衝突は残ります。  
`shlib_variant` を使うと、ファイル名、SONAME、シンボル バージョンが同時に変わります。  
使わない場合は、OpenSSL が作った `libcrypto.so.3` を別名で配置する方法になり、SONAME とシンボル バージョンは OpenSSL の標準のままです。

```text
案 A: shlib_variant => "-app" を指定
  ファイル名と SONAME: libcrypto-app.so.3
  シンボル バージョン: OPENSSL_APP_3.0.0
  システムの OpenSSL 3.x と同じプロセスに載っても、呼び出しは混ざらない

案 B: ファイル名だけを変更
  SONAME: libcrypto.so.3 のまま (利用側には SONAME が記録されるため、実行時は libcrypto.so.3 を探す)
  シンボル バージョン: OPENSSL_3.0.0 のまま
  システムの OpenSSL 3.x と区別されない
```

案 B では、利用側に記録される名前が SONAME の `libcrypto.so.3` になるため、ファイル名を変えた効果が実行時に及びません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `shlib_variant` を使い、ファイル名、SONAME、シンボル バージョンを変える。
- [ ] B: ファイル名だけを変える。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-12 06:47:27 JST (検討用 Markdown の保存時刻)

**Assessment**

`shlib_variant` を採用することが確定しました。動作は作業 WBS の 1.5 の事前検証で確認します。

**Recommendation**

推奨: A  
理由: OpenSSL が文書化している共存用の設定であり、本体へのパッチが不要です。現在の静的リンクが防いでいるシンボル衝突を、共有ライブラリでも防げます。  
制約: 実際のビルドでの確認は未実施です ([Q-008](#q-008-第-1-ゲート前の事前検証))。

**Related**

[Q-003](#q-003-名前の形式)、[Q-006](#q-006-cplat-製品ビルドの-openssl-リンク方法)、[Q-008](#q-008-第-1-ゲート前の事前検証)

### Q-003 名前の形式

**Status**

解決済み

**Question**

第三者管理ライブラリの新しい名前は、どの形式にしますか。

**Context**

依頼の例は、接頭辞を付ける `libapp_openssl` でした。  
一方、[Q-002](#q-002-openssl-のシンボル衝突対策) の `shlib_variant` は、ライブラリ名の後ろに識別子を付ける設定であり、接頭辞は付けられません。  
OpenSSL の成果物は libcrypto と libssl の 2 つで、`libapp_openssl` という 1 つのライブラリにはなりません。  
「すべて同じ規則」を保つには、全ライブラリを接尾辞にそろえる必要があります。

```text
案 A: 全ライブラリを接尾辞 "-app" にそろえる
  libzlib-app.so / libcjson-app.so / liblua-app.so / libsqlite3-app.so
  libcrypto-app.so.3 / libssl-app.so.3
  リンク指定: LIBS += zlib-app、LIBS += ssl-app crypto-app

案 B: 全ライブラリを接尾辞 "_app" にそろえる
  libzlib_app.so / libcjson_app.so / liblua_app.so / libsqlite3_app.so
  libcrypto_app.so.3 / libssl_app.so.3
  リンク指定: LIBS += zlib_app、LIBS += ssl_app crypto_app

案 C: makefw 製は接頭辞、OpenSSL だけ接尾辞
  libapp_zlib.so / libapp_cjson.so / libapp_lua.so / libapp_sqlite3.so
  libcrypto-app.so.3 / libssl-app.so.3
  リンク指定: LIBS += app_zlib、LIBS += ssl-app crypto-app

案 D: 全ライブラリを接頭辞にそろえる
  libapp_zlib.so、libapp_crypto.so など
  OpenSSL は shlib_variant では実現できず、本体のビルド定義へのパッチが必要
```

案 D は、app/openssl の AGENTS.md にある「OpenSSL 本体にはパッチを当てない」という規則と相反します。

**Answer**

選択方式: 単一選択

- [ ] A: 全ライブラリを接尾辞 `-app` にそろえる。
- [x] B: (推奨) 全ライブラリを接尾辞 `_app` にそろえる。
- [ ] C: makefw 製は接頭辞 `app_`、OpenSSL だけ接尾辞とする。
- [ ] D: 全ライブラリを接頭辞 `app_` にそろえ、OpenSSL にはパッチを当てる。
- [ ] E: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-12 06:47:27 JST (検討用 Markdown の保存時刻)

**Assessment**

全ライブラリを接尾辞 `_app` にそろえることが確定しました。`shlib_variant` に `_app` を指定できるかは、作業 WBS の 1.5 の事前検証で確認します。指定できない場合は、新しい Q で形式を再確認します。

**Recommendation**

推奨: B  
理由: 規則が 1 つになり、OpenSSL へのパッチも不要です。既存のライブラリ名 (`libstruct_meta.so`、`libmock_zlib.a`) が区切りに下線を使っているため、`_app` が既存の名前とそろいます。  
制約: `shlib_variant` に下線で始まる値を指定できるかは、ビルドでの確認が必要です ([Q-008](#q-008-第-1-ゲート前の事前検証))。確認で不可と分かった場合は、案 A を改めて確認します。

**Related**

[Q-002](#q-002-openssl-のシンボル衝突対策)、[Q-004](#q-004-mock-ライブラリの名前)

### Q-004 mock ライブラリの名前

**Status**

解決済み

**Question**

mock ライブラリ (`libmock_zlib.a`、`libmock_cjson.a`、`libmock_lua.a`、`libmock_sqlite3.a`、`libmock_openssl.a`) の名前は、現在のままとしますか。

**Context**

mock ライブラリは、このワークスペースで作成した静的ライブラリです。  
システムに同名のライブラリはなく、名前の重複は起きません。  
一方、mock の名前は差し替える対象のライブラリ名に対応しているため、対象の名前に合わせて変える考え方もあります。

```text
案 A: 変えない
  LIBS += mock_zlib

案 B: 対象の新しい名前に合わせる ([Q-003](#q-003-名前の形式) が案 B の場合の例)
  LIBS += mock_zlib_app
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) mock ライブラリの名前は変えない。
- [ ] B: 対象ライブラリの新しい名前に合わせて変える。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-12 06:47:27 JST (検討用 Markdown の保存時刻)

**Assessment**

mock ライブラリの名前を変えないことが確定しました。

**Recommendation**

推奨: A  
理由: mock は第一者が作成したライブラリであり、「第一者管理ライブラリの名前は変えない」という前提に合います。利用側の変更も少なくなります。

**Related**

[Q-003](#q-003-名前の形式)

### Q-005 OpenSSL の mock の方式

**Status**

解決済み

**Question**

共有ライブラリ化した後の OpenSSL の mock は、どの方式にしますか。

**Context**

現在の mock は、テスト対象が取り込む OpenSSL のヘッダーを差し替え、13 個の関数の呼び出しをマクロで mock 関数へ置き換えています。  
この方式は GCC の `#include_next` に依存し、Linux だけで動作します。  
mock 関数は、静的な libcrypto の実関数へ委譲しています。

zlib などの mock は、公開関数と同じ名前の関数を mock ライブラリが定義し、実ライブラリを実行時に読み込んで委譲します。  
利用側は mock だけをリンクし、実ライブラリを同時にリンクしません (`app/general/docs/shared-library-mock-guideline.md` の「リンク規則」)。

OpenSSL の公開関数は約 6,500 個あり、zlib と同じ方式で全関数を載せると、mock のビルド時間とデバッグ情報が大きくなります。

```text
案 A: 部分的な API 表 + 実ライブラリの同時リンク
  API 表には、失敗を注入したい関数だけを載せる (現在の 13 個から開始)
  表にない関数は、同時にリンクした実ライブラリから解決する
  現行規範「実ライブラリと mock を同時リンクしない」の見直しが必要
  Windows で同時リンクが成立するかは未確認

案 B: 全関数の API 表
  util/libcrypto.num と util/libssl.num から約 6,500 個の表を生成する
  現行規範のまま利用できる
  mock のビルド時間とサイズが大きくなる

案 C: 現在のヘッダー差し替え方式を維持
  委譲先だけを共有ライブラリの実関数に変える
  Linux 専用のまま
```

**Answer**

選択方式: 単一選択

- [ ] A: 部分的な API 表とし、実ライブラリを同時にリンクする。
- [ ] B: 全関数の API 表を生成する。
- [ ] C: 現在のヘッダー差し替え方式を維持し、委譲先だけを変える。
- [x] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: 現在の app で利用している分だけ (部分的な API 表) を mock し、mock からの委譲のみで構成する。利用したい API が mock されていない場合は、都度追加する運用とすれば cplat などと同じ方式にできるのではないか。

回答者: Tetsuo Honda  
回答日時: 2026-11-12 06:47:27 JST (検討用 Markdown の保存時刻)

**Assessment**

部分的な API 表とし、利用側は mock だけをリンクして、実関数へは mock から委譲する方式が確定しました。  
この方式は、現行規範の「実ライブラリと mock を同時リンクしない」と整合します。  
mock にない関数を利用側が呼ぶとリンク時に未解決となるため、追加が必要な関数はビルドで検出できます。  
現在のヘッダー差し替え (`test/include_override/`) は不要になり、Windows でも mock を利用できる見込みです。  
mock が定義する関数と同名の関数を、実ライブラリの内部呼び出しが mock 側へ解決しないことは、作業 WBS の 1.5 の事前検証で確認します。

**Recommendation**

推奨: なし  
理由: 案 A は Windows での成立性が未確認であり、案 B はビルド時間とサイズを測定していません。案 A と案 B のどちらが現実的かは、[Q-008](#q-008-第-1-ゲート前の事前検証) の事前検証の結果で判断できます。事前検証の前に方式を決める場合は、目的 (他の mock と同じ方式で、Windows でも使えること) に照らして選択してください。

**Related**

[Q-008](#q-008-第-1-ゲート前の事前検証)、[Q-011](#q-011-openssl-の-mock-の呼び出し元情報)、`app/general/docs/shared-library-mock-guideline.md`

### Q-006 cplat 製品ビルドの OpenSSL リンク方法

**Status**

解決済み

**Question**

libcplat.so の製品ビルドも、OpenSSL を動的リンクに変えますか。

**Context**

現在の libcplat.so は、`libcrypto.a` を内部に取り込んでおり、単体で配布できます。  
動的リンクに変えると、libcplat.so の配布に libcrypto の共有ライブラリ (数 MB) の同梱が必要になります。  
OpenSSL の脆弱性対応では、動的リンクなら libcrypto の差し替えだけで済み、静的リンクなら libcplat.so の再ビルドが必要です。

```text
案 A: 動的リンクに一本化
  app/openssl は共有ライブラリだけを作る
  libcplat.so は libcrypto の共有ライブラリに依存する

案 B: 製品は静的リンクのまま、共有ライブラリは mock の委譲先としてだけ使う
  app/openssl は静的ライブラリと共有ライブラリの両方を作る (ビルド時間が増える)
  テストで委譲する実関数と、製品が使う実関数が別のバイナリになる
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 製品ビルドも動的リンクに変え、app/openssl は共有ライブラリだけを作る。
- [ ] B: 製品ビルドは静的リンクのままとし、app/openssl は両方を作る。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-12 06:47:27 JST (検討用 Markdown の保存時刻)

**Assessment**

製品ビルドも動的リンクに変え、app/openssl は共有ライブラリだけを作ることが確定しました。前提とした [Q-002](#q-002-openssl-のシンボル衝突対策) も案 A で確定しています。

**Recommendation**

推奨: A  
理由: zlib や cjson と同じ構成になり、テストと製品が同じバイナリを使います。  
制約: [Q-002](#q-002-openssl-のシンボル衝突対策) で案 A (`shlib_variant`) を採用することが前提です。採用しない場合は、動的リンクにするとシンボル衝突の防御がなくなります。

**Related**

[Q-002](#q-002-openssl-のシンボル衝突対策)

### Q-007 makefw 製ライブラリのシンボル衝突対策の扱い

**Status**

解決済み

**Question**

zlib、cjson、lua、sqlite のシンボル名の衝突への対策は、今回の対象外としますか。

**Context**

これらの共有ライブラリには SONAME もシンボル バージョンもありません。  
たとえば、システムの `libz.so.1` を読み込むプロセスに、このワークスペースの zlib を載せると、`deflate` などの呼び出しが混ざる可能性があります。  
この問題は現在の構成にすでにあり、名前の変更では悪化も改善もしません。  
対策するには、makefw に SONAME とバージョン スクリプトを付ける機能を追加することになり、makefw を使う全 app に影響します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 今回の対象外とし、残課題一覧に記録する。
- [ ] B: 今回の対象に含める。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-12 06:47:27 JST (検討用 Markdown の保存時刻)

**Assessment**

今回の対象外とすることが確定しました。残課題一覧 (`261111-third-party-lib-rename-backlog.md`) に記録します。

**Recommendation**

推奨: A  
理由: 今回の目的 (OpenSSL の共有ライブラリ化と mock の実委譲) には必要がなく、makefw の機能追加は影響範囲が別です。

**Related**

[Q-002](#q-002-openssl-のシンボル衝突対策)

### Q-008 第 1 ゲート前の事前検証

**Status**

解決済み

**Question**

最終仕様を作る前に、`shlib_variant` を指定した OpenSSL の共有ビルドを、セッション用の一時フォルダーで試しますか。

**Context**

`shlib_variant` の動作は、OpenSSL の文書とスクリプトの読解で確認しただけです。  
確認したい点は次のとおりです。

- 下線で始まる値 (`_app`) を指定できること ([Q-003](#q-003-名前の形式))
- SONAME、シンボル バージョン、Windows の DLL 名が想定どおりになること ([Q-002](#q-002-openssl-のシンボル衝突対策))
- mock と実ライブラリの同時リンクが、Linux と Windows で成立すること ([Q-005](#q-005-openssl-の-mock-の方式))

一時フォルダーへ配布アーカイブを展開してビルドするため、リポジトリのファイルは変更しません。  
Linux のビルドは数分、Windows のビルドは nmake が並列化できないため、より長くかかります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) Linux と Windows の両方で試す。
- [ ] B: Linux だけで試し、Windows は作業の中で確認する。
- [ ] C: 事前検証は行わず、作業の中で確認する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-12 06:47:27 JST (検討用 Markdown の保存時刻)

**Assessment**

Linux と Windows の両方で事前検証を行うことが確定しました。[Q-005](#q-005-openssl-の-mock-の方式) の回答により、同時リンクの確認は不要になり、代わりに mock だけをリンクする実委譲を確認します。

**Recommendation**

推奨: A  
理由: [Q-003](#q-003-名前の形式) と [Q-005](#q-005-openssl-の-mock-の方式) の判断が検証結果に左右されます。作業の途中で前提が崩れると、名前を変えた全 app のやり直しになります。

**Related**

[Q-002](#q-002-openssl-のシンボル衝突対策)、[Q-003](#q-003-名前の形式)、[Q-005](#q-005-openssl-の-mock-の方式)

### Q-009 移行の進め方

**Status**

解決済み

**Question**

対象 app の名前変更と OpenSSL の共有ライブラリ化は、1 つの作業としてまとめて行いますか。

**Context**

名前を変えると、利用側 (cplat、porter、struct-meta など) のリンク指定も同時に変える必要があります。  
各 app は別リポジトリ (サブモジュール) で管理されているため、変更は複数のリポジトリにまたがります。

```text
案 A: 一括
  全対象 app の名前変更、利用側の更新、OpenSSL の共有ライブラリ化を 1 つの作業で行う

案 B: 2 段階
  第 1 段階: makefw 製の 4 つの app の名前変更と利用側の更新
  第 2 段階: OpenSSL の共有ライブラリ化、mock の実委譲、cplat のリンク方法の変更
```

**Answer**

選択方式: 単一選択

- [ ] A: 一括で行う。
- [x] B: (推奨) 2 段階で行う。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-12 06:47:27 JST (検討用 Markdown の保存時刻)

**Assessment**

2 段階で行うことが確定しました。作業 WBS の項目 4 は、第 1 段階 (4.1 から 4.3) と第 2 段階 (4.4 から 4.6) の順に実施します。

**Recommendation**

推奨: B  
理由: 第 1 段階は機械的な名前の置き換えで、既存のテストで確認できます。第 2 段階はビルド方式と mock 方式が変わるため、失敗した場合の原因を切り分けやすくなります。どちらの段階も、この検討の範囲として最後まで実施します。

**Related**

[Q-001](#q-001-対象とする第三者管理ライブラリ)、[Q-010](#q-010-受入検証の範囲)

### Q-010 受入検証の範囲

**Status**

解決済み

**Question**

作業完了の確認として、どの範囲のビルドとテストを実行しますか。

**Context**

ルートの AGENTS.md は、複数の app にまたがる全体テストについて、コストを説明してからユーザーに確認することを求めています。  
今回の変更は、対象の 5 つの app と、それらを利用する app (cplat、porter、struct-meta など) のリンクに影響します。  
OpenSSL の再ビルドを含むため、全体のビルドとテストは通常より時間がかかります。  
Windows のビルドは、この Linux 環境からは実行できない場合があります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) Linux で、対象 app と利用側 app のビルドとテストを実行する。Windows は CI の結果で確認する。
- [ ] B: Linux と Windows の両方で、対象 app と利用側 app のビルドとテストをローカルで実行する。
- [ ] C: Linux で、ワークスペース全体のビルドとテストを実行する。Windows は CI の結果で確認する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-12 06:47:27 JST (検討用 Markdown の保存時刻)

**Assessment**

Linux で対象 app と利用側 app のビルドとテストを実行し、Windows は CI で確認することが確定しました。CI に必要なコミットとプッシュは、ユーザーの指示を受けてから行います。

**Recommendation**

推奨: A  
理由: 変更の影響はリンク関係のある app に限られます。Windows の確認を CI に任せる場合、CI の実行にはコミットとプッシュが必要であり、その操作はユーザーの指示を受けてから行います。

**Related**

[Q-009](#q-009-移行の進め方)

### Q-011 OpenSSL の mock の呼び出し元情報

**Status**

解決済み

**Question**

OpenSSL の mock で、呼び出し元のファイル名、行番号、関数名を受け取る先頭 3 引数を廃止しますか。

**Context**

最終仕様の版 3 に基づく案は、[Q-005](#q-005-openssl-の-mock-の方式) の回答を受けて、OpenSSL の mock を zlib などと同じ方式 (同名の関数を mock が定義する方式) に変えるものでした。  
この方式では、呼び出し元の情報を渡すマクロ置換がなくなるため、先頭 3 引数を廃止する内容になっていました。  
第 2 ゲートの承認確認で、この点が差し戻されました。

**Answer**

選択方式: 単一選択

- [ ] A: 先頭 3 引数を廃止する。
- [x] B: 先頭 3 引数を維持する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: 「先頭 3 引数で呼び出し元のファイル名、行番号、関数名は、テストの Assert に有効であり、他の第三者管理ライブラリのモックに機能がないことが問題であり、backlog 化し openssl の mock では 3 引数を維持すること。」 (出典: 2026-11-12 の対話セッションでのユーザーの発言)

回答者: Tetsuo Honda  
回答日時: 2026-11-12 (対話セッション。時刻は記録なし)

**Assessment**

先頭 3 引数を維持することが確定しました。  
呼び出し元の情報を渡すには、テスト対象が取り込む OpenSSL のヘッダーを差し替え、呼び出しをマクロで mock 関数へ置き換える現在の仕組みが必要です。  
このため、ヘッダーの差し替えと `mock_<関数名>` の形は維持し、[Q-005](#q-005-openssl-の-mock-の方式) で確定した次の 3 点をこの仕組みの上で実現します。

- 利用している関数だけを対象にすること
- 利用側は mock だけをリンクすること
- 実関数へは mock から委譲すること

mock だけをリンクするため、テスト対象が呼ぶ OpenSSL の関数は、失敗を注入しない関数 (`EVP_sha256` など) も含めてすべて置き換えの対象にします。

現在の差し替えヘッダーは GCC の `#include_next` に依存します。  
Windows では、testfw の標準ライブラリ mock と同じく、include パスからの相対パスで実ヘッダーを取り込む方法が使えることを、一時フォルダーで確認しました (呼び出し元の情報が mock へ渡り、mock だけのリンクで実委譲が成立)。

追加の確認で、現在の差し替えヘッダーに取り込み順序の依存があることが分かりました。  
1 つのソースが `<openssl/evp.h>`、`<openssl/rand.h>` の順に取り込むと、先に定義された置換マクロが `rand.h` の関数宣言を書き換え、コンパイル エラーになります。  
最終仕様では、置換マクロを定義する前に、対象の実ヘッダーをすべて取り込む形に改めます。

ほかの第三者管理ライブラリ (zlib、cjson、lua、sqlite) の mock に呼び出し元の情報がないことは、残課題一覧の BL-002 に記録しました。

**Related**

[Q-005](#q-005-openssl-の-mock-の方式)

## 事前検証の結果

[Q-008](#q-008-第-1-ゲート前の事前検証) に従い、2026-11-12 に一時フォルダーで実施しました。  
リポジトリのファイルは変更していません。

| 確認項目 | Linux (GCC) | Windows (MSVC) |
| --- | --- | --- |
| `shlib_variant` に `_app` を指定した共有ビルド | 成功 | 成功 (約 11 分) |
| 共有ライブラリのファイル名 | `libcrypto_app.so.3`、`libssl_app.so.3` | `libcrypto-3-x64_app.dll`、`libssl-3-x64_app.dll` |
| SONAME | ファイル名と同じ | 対象外 |
| シンボル バージョン | `OPENSSL_APP_3.0.0` など | 対象外 |
| OpenSSL が作るリンク用のファイル名 | `libcrypto.so`、`libssl.so` (シンボリック リンク) | `libcrypto.lib`、`libssl.lib` (import library) |
| mock だけをリンクした実行ファイルからの実委譲 | 成立 | 成立 |
| 実ライブラリの内部呼び出しが mock の同名関数へ再入しないこと | 再入なし | 対象外 (DLL ごとに名前空間が分かれる) |
| システムの OpenSSL 1.1.1 を使う共有ライブラリとの同居 | それぞれが自分の版を呼ぶ | 対象外 |

Table: 事前検証の結果

検証で分かった点は次のとおりです。

- リンク用のファイル名には `_app` が付きません。`LIBS += crypto_app` で解決させるには、配置時に `libcrypto_app.so` (Linux) と `libcrypto_app.lib` (Windows) の名前で用意する必要があります。Windows では、import library の名前を変えても、参照する DLL 名は `libcrypto-3-x64_app.dll` のままであることを確認しました。
- Windows の DLL 名は、OpenSSL の命名 (`-3-x64`) の後ろに `_app` が付きます。パッチなしでは、この形を変えられません。
- システム側が OpenSSL 3.x の環境での同居は、この開発環境 (システム側は 1.1.1) では確認できていません。

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261111-third-party-lib-rename-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 4

最終仕様 (design.md) の合意版 (第 2 ゲート): 2026-11-12 承認 (「OpenSSL の mock」改訂後の版)

更新予定の成果物: `261111-third-party-lib-rename-design.md` の「変更対象」に記載

受入条件: `261111-third-party-lib-rename-design.md` の「受入条件」に記載

## 反映と検証の記録

2026-11-12 に codex が既存の未コミット変更を確認して再開しました。ステージング、コミット、プッシュは行っていません。

### 実装と局所検証

- zlib の `TARGET := zlib_app` は、Linux で clean 後に `libzlib_app.so` を再生成し、続く make で再リンク不要となることを確認しました。Windows 条件の make の展開では `libzlib_app.dll` と import library `libzlib_app.lib`、clean 対象への反映を確認しました。Windows での実ビルド、依存関係の追跡、clean の実動作は未確認であり、4.1 は要注意です。
- cplat、porter、struct-meta のリンク指定と、cplat の同梱定義を新名へ変更しました。Linux の同梱対象に `libcrypto_app.so.3` を追加しました。porter の送受信テストが子プロセスへ渡す固定の `LD_LIBRARY_PATH` にも、OpenSSL の配置先を追加しました。送受信テスト 43 件は、ローカル通信を許可した再実行ですべて成功しました。サンドボックス内ではソケットを開けませんでした。
- OpenSSL のビルドと、文字コード警告をエラー扱いにした Python の 34 テストが成功しました。配置済みの libcrypto と libssl の SONAME、および `OPENSSL_APP_*` のシンボル バージョンを確認しました。
- cplat の `NEEDED` に `libcjson_app.so`、`libzlib_app.so`、`libcrypto_app.so.3` があることを確認しました。cryptoTest 35 件と randomTest 6 件は、実ライブラリを同時リンクしない状態で成功しました。
- `evp.h` と `rand.h` をどちらの順で取り込んでも、差し替えヘッダーを通した C のコンパイルが成功しました。一時的な C/C++ の検証プログラムでは、`EVP_sha256` の mock に呼び出し元のファイル名、正の行番号、関数名が渡ることを照合しました。
- cJSON、Lua、SQLite のビルドとテストが成功しました。zlib のテストは、依存関係が同一であることによる既存成功結果の再利用を確認しました。struct-meta のビルドとテストも成功しました。
- 実行中シェルの `LD_LIBRARY_PATH` には OpenSSL の追加前の値が残っていました。crypto 関連テストの初回実行はライブラリの探索に失敗し、追加後の探索パスを設定した再実行は成功しました。`sync-app-env.sh --check` は同期済みと判定しました。
- 旧名の cJSON、Lua、SQLite の生成ライブラリと、実行ファイルの配置先に残っていた旧名の同梱物は `/tmp/third-party-old-libraries/` へ退避しました。
- service-sample と tutorial の製品ビルドが成功しました。`LD_LIBRARY_PATH` を外した `ldd` で、同梱した cplat、cJSON、zlib、OpenSSL が同じディレクトリから解決されることを確認しました。
- Gemini 4 Argon による文書の読み取りレビューは「問題なし」でした。文章規範に従い、変更した Markdown に `text_style_jp.py --dry-run` と `--in-place` を実行しました。

### 最終確認 (claude code、2026-11-12)

- cplat のビルドと全テストが成功しました (失敗 0 件)。porter のビルドと全テストが成功しました (失敗 0 件)。
- 対象の 5 つの app と cplat、porter、struct-meta に、内容のある `.warn` ファイルはありません。
- OpenSSL のビルド スクリプトのテスト 34 件が成功しました。
- 成果物名が最終仕様の「命名規則」の表と一致し、`prod/lib/` に変更前の名前の成果物がないことを確認しました。
- `libcplat.so` が `libcrypto_app.so.3` に依存し、OS の libcrypto に依存しないことを確認しました。
- 差し替えヘッダーは、`evp.h` と `rand.h` をどちらの順で取り込んでもコンパイルでき、呼び出しが呼び出し元情報付きの `mock_<関数名>` に置き換わることを確認しました。
- 変更前の名前のリンク指定と成果物名の参照が、管理対象のファイルに残っていないことを確認しました。
- 静的リンクを前提とした記述が残っていた 3 か所 (`app/openssl/packages/README.md`、`app/cplat/appdeps.mk`、`framework/testfw/docs/how-to-mock.md`) を直しました。`how-to-mock.md` は最終仕様の「正本ドキュメントへの反映」に挙げていない文書ですが、差し替え方式を使う理由の記述が事実と合わなくなるため直しました。
- 変更した Markdown は `text_style_jp.py --dry-run` で指摘なしです。
- 残課題一覧に BL-003 (OS 側が OpenSSL 3.x の環境での同居の確認) を追加しました。

### 未確認の事項

Windows のビルドとテストは未実施です。[Q-010](#q-010-受入検証の範囲) のとおり CI で確認します。  
事前検証では、Windows の一時フォルダーで OpenSSL の共有ビルド、mock だけのリンク、差し替えヘッダーを個別に確認しています。  
makefw の `TARGET` 指定による Windows の成果物名、書き換えたビルド スクリプトの Windows の配置処理、Windows での mock ライブラリのビルドは、CI が最初の実行になります。
