<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: claude code / セッション ID: session_01KJc53RRnu4roDTnMUeKvMT
上流文書: `261111-third-party-lib-rename-discussion.md` (検討用 Markdown)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 第三者管理ライブラリの名前変更と OpenSSL の共有ライブラリ化 最終仕様

ファイル名: `261111-third-party-lib-rename-design.md`

## 目的

app/openssl を共有ライブラリにし、OpenSSL の mock が、利用側と同時にリンクした実ライブラリではなく、実行時に読み込んだ実ライブラリへ既定で委譲するようにします。  
共有ライブラリにしても OS やほかの製品の同名ライブラリと取り違えないよう、第三者管理ライブラリの名前を 1 つの規則で変更します。

## 命名規則

第三者管理ライブラリ (外部の配布アーカイブから取り込むライブラリ) の名前には、接尾辞 `_app` を付けます。  
第一者管理ライブラリ (このワークスペースで作成するライブラリ) と mock ライブラリの名前は変更しません。

| app | 変更前 (Linux) | 変更後 (Linux) | 変更後 (Windows) | リンク指定 |
| --- | --- | --- | --- | --- |
| zlib | `libzlib.so` | `libzlib_app.so` | `libzlib_app.dll`、`libzlib_app.lib` | `LIBS += zlib_app` |
| cjson | `libcjson.so` | `libcjson_app.so` | `libcjson_app.dll`、`libcjson_app.lib` | `LIBS += cjson_app` |
| lua | `liblua.so` | `liblua_app.so` | `liblua_app.dll`、`liblua_app.lib` | `LIBS += lua_app` |
| sqlite | `libsqlite3.so` | `libsqlite3_app.so` | `libsqlite3_app.dll`、`libsqlite3_app.lib` | `LIBS += sqlite3_app` |
| openssl | `libcrypto.a` | `libcrypto_app.so.3` | `libcrypto-3-x64_app.dll`、`libcrypto_app.lib` | `LIBS += crypto_app` |
| openssl | `libssl.a` | `libssl_app.so.3` | `libssl-3-x64_app.dll`、`libssl_app.lib` | `LIBS += ssl_app crypto_app` |

Table: 第三者管理ライブラリの変更前後の名前

> [!NOTE]
> OpenSSL の共有ライブラリ名は、OpenSSL のビルド システムが決めます。  
> Linux では版の番号 (`.3`) が付き、Windows では `-3-x64` の後ろに `_app` が付きます。  
> OpenSSL 本体にパッチを当てない限り、この形は変えられません。

## makefw 製ライブラリの名前変更 (第 1 段階)

対象は zlib、cjson、lua、sqlite の 4 つの app です。

### ライブラリ名の指定

各 app の `prod/libsrc/<ディレクトリ>/makepart.mk` で、`TARGET` に新しい名前 (`zlib_app` など) を指定します。  
makefw は `TARGET` が未指定のときにディレクトリ名をライブラリ名にするため、ソースのディレクトリ名は変更しません。

> [!IMPORTANT]
> `TARGET` を `makepart.mk` で指定している app は、現在ありません。  
> 作業の最初に 1 つの app で、Linux と Windows の成果物名、依存関係の追跡、clean が正しく動くことを確認します。  
> 正しく動かない場合は作業を止め、方法を改めて確認します。

### 追従する変更

- 各 app の mock が実委譲に使うライブラリ名の定数 (`test/include/mock_*.h` の `kLib*Name`)
- 各 app の公開シンボル検査 (`test/src/lib*Test/exportTest/`) が開くライブラリ名
- 各 app と利用側 app の `makepart.mk` のリンク指定 (`LIBS`)
- `app/cplat/prod/runtime-bundle.mk` が実行ファイルと同じディレクトリへコピーするファイル名
- 各 app の README.md、AGENTS.md と、`app/cplat/docs/link-policy.md` などの記載

利用側 app は、cplat、porter、struct-meta、および対象 app 自身のコマンドとテストです。  
作業時に全 app を検索し、変更前の名前の参照が残らないことを確認します。

## OpenSSL の共有ライブラリ化 (第 2 段階)

### ビルド

`app/openssl/bin_internal/build_openssl.py` を次のように変更します。

- OpenSSL の `shlib_variant` に `_app` を指定します。指定は、ターゲット定義ファイルを追加して Configure の `--config` で渡す方法で行い、OpenSSL 本体にはパッチを当てません。
- ターゲット定義は、既存のターゲット (`linux-x86_64`、`VC-WIN64A` など、現在対応しているもの) を継承し、`shlib_variant` だけを追加します。
- Configure の引数を、`no-shared` から `shared` へ変更します。`no-pinshared` は外します。
- 静的ライブラリは配置しません。

`shlib_variant` の指定により、次の 3 つが同時に変わります。

- 共有ライブラリのファイル名
- SONAME (Linux)
- シンボル バージョン (Linux)。`OPENSSL_3.0.0` が `OPENSSL_APP_3.0.0` になります。

> [!NOTE]
> Linux では、ライブラリ名が違っても関数名はプロセス全体で 1 つの名前空間に入ります。  
> シンボル バージョンを変えることで、このライブラリを使う呼び出しと、OS の OpenSSL を使う呼び出しが、同じプロセスの中でも互いに混ざらなくなります。  
> 静的リンクと `--exclude-libs` で実現していた分離を、共有ライブラリではこの方法で実現します。

### 配置

`prod/lib/` に次のファイルを配置します。

| 環境 | 実行時に使うファイル | リンク時に使うファイル |
| --- | --- | --- |
| Linux | `libcrypto_app.so.3`、`libssl_app.so.3` | `libcrypto_app.so`、`libssl_app.so` (上記へのシンボリック リンク) |
| Windows | `libcrypto-3-x64_app.dll`、`libssl-3-x64_app.dll` | `libcrypto_app.lib`、`libssl_app.lib` (import library) |

Table: OpenSSL の配置物

> [!NOTE]
> OpenSSL のビルドが作るリンク用のファイル名は、`libcrypto.so` と `libcrypto.lib` であり、`_app` が付きません。  
> `LIBS += crypto_app` で解決できるよう、配置のときに名前を変えます。  
> Windows の import library は、名前を変えても参照する DLL 名は変わりません。

OpenSSL のプロバイダー モジュール (`legacy` など) とエンジンは配置しません。  
既定のプロバイダーは libcrypto に含まれます。

### ビルド結果の検査

`build_openssl.py` の検査を、共有ライブラリに合わせて置き換えます。

- 静的ライブラリだけであることの検査 (`assert_no_shared`) を廃止します。
- Linux では、SONAME が `libcrypto_app.so.3` と `libssl_app.so.3` であること、および公開シンボルのバージョンが `OPENSSL_APP_` で始まることを検査します。
- Windows では、DLL 名に `_app` が含まれることを検査します。

`bin_test/test_build_openssl.py` を、これらの変更に合わせて更新します。

### 利用側の指定

- リンク指定は `LIBS += crypto_app`、TLS を使う場合は `LIBS += ssl_app crypto_app` とします。
- 利用側が共有ライブラリの場合の `--exclude-libs` の指定は不要になります。
- 公開ヘッダーの取り込み方 (system include) は変更しません。

## OpenSSL の mock

### 方式

現在の仕組み (ヘッダーの差し替えとマクロによる置き換え) を維持し、リンクと委譲の方法を変更します。

- テスト対象 (`TEST_SRCS`) が取り込む OpenSSL のヘッダーを `test/include_override/openssl/` の差し替えヘッダーで置き換え、OpenSSL の関数の呼び出しを、マクロで `mock_<関数名>` へ置き換えます。
- `mock_<関数名>` の先頭 3 引数は、呼び出し元のファイル名、行番号、関数名です。`Mock_openssl` の `MOCK_METHOD` も同じ引数を持ち、テストの検証に使えます。
- mock を生成していない場合と、個別の動作を設定していない場合は、実ライブラリ (`libcrypto_app`) の関数へ委譲します。実関数は、testfw の `resolveSharedSymbolOrExit()` で実行時に解決します。
- 利用側のテストは mock (`libmock_openssl.a`) だけをリンクし、実ライブラリを同時にリンクしません。
- Linux と Windows の両方でビルドします。

> [!NOTE]
> 呼び出し元の情報は、テストで「どこから呼ばれたか」を検証するために使います。  
> 同名の関数を mock が定義する方式 (zlib などの mock の方式) では、呼び出し元の情報を渡せません。

### 置き換える関数

置き換えの対象は、このワークスペースの app のテスト対象が呼ぶ OpenSSL の関数のすべてです。  
利用側が mock だけをリンクするため、失敗を注入しない関数も対象にします。  
最初の対象は、cplat が利用している次の 17 個です。

- `EVP_CIPHER_CTX_new`、`EVP_CIPHER_CTX_free`、`EVP_CIPHER_CTX_ctrl`
- `EVP_EncryptInit_ex`、`EVP_EncryptUpdate`、`EVP_EncryptFinal_ex`
- `EVP_DecryptInit_ex`、`EVP_DecryptUpdate`、`EVP_DecryptFinal_ex`
- `EVP_MD_CTX_new`、`EVP_MD_CTX_free`
- `EVP_DigestInit_ex`、`EVP_DigestUpdate`、`EVP_DigestFinal_ex`
- `EVP_aes_256_gcm`、`EVP_sha256`
- `RAND_bytes`

現在の mock が置き換えている 13 個に、`EVP_CIPHER_CTX_free`、`EVP_MD_CTX_free`、`EVP_aes_256_gcm`、`EVP_sha256` の 4 個を加えます。

app が新しい OpenSSL の関数を使う場合は、その都度、置き換えの対象へ追加します。  
対象にない関数をテスト対象が呼ぶと、テストのリンクが未解決シンボルで失敗するため、追加が必要な関数はビルドで検出できます。

> [!NOTE]
> OpenSSL の公開関数は、libcrypto と libssl を合わせて約 6,500 個あります。  
> 全関数を対象にすると mock のビルド時間とサイズが大きくなるため、利用している関数だけを対象にします。

### 差し替えヘッダー

差し替えヘッダー (`evp.h`、`rand.h`) を次のように改めます。

- 置換マクロを定義する前に、mock が対象とする実ヘッダー (`evp.h` と `rand.h`) をすべて取り込みます。
- Linux では `#include_next` で、Windows では include パスからの相対パスで、実ヘッダーを取り込みます。

> [!NOTE]
> 置換マクロを先に定義すると、後から取り込まれる実ヘッダーの関数宣言がマクロで書き換えられ、コンパイル エラーになります。  
> 現在のヘッダーは、1 つのソースが `<openssl/evp.h>`、`<openssl/rand.h>` の順に取り込む場合にこの問題が起きます。  
> Windows の取り込み方は、testfw の標準ライブラリ mock (`framework/testfw/include_override/stdio.h` など) と同じ考え方です。

### 制約

- 関数形式マクロによる置き換えは、関数を関数ポインターとして渡す使い方には効きません。その場合は実関数への参照が残り、mock だけのリンクでは未解決になります。現在の利用箇所に、この使い方はありません。
- テスト コード自身 (`TEST_SRCS` 以外) は置き換えの対象外です。テスト コードから OpenSSL の関数を直接呼ぶ必要が生じた場合は、`mock_<関数名>` を呼びます。現在のテスト コードに、直接の呼び出しはありません。

### 実ライブラリの名前

mock が実委譲で開くライブラリ名は、`mock_openssl.h` に OS ごとの定数として定義します。

| 環境 | ライブラリ名 |
| --- | --- |
| Linux | `libcrypto_app.so.3` |
| Windows | `libcrypto-3-x64_app.dll` |

Table: mock が開く実ライブラリの名前

## cplat の変更

- `libcplat.so` (Linux) は、`libcrypto_app.so.3` を動的にリンクします。`prod/libsrc/cplat/makepart.mk` のリンク指定を `crypto_app` に変更します。
- Windows の cplat は、これまでどおり CNG を使い、OpenSSL をリンクしません。
- `prod/runtime-bundle.mk` の同梱対象に、Linux では `libcrypto_app.so.3` を追加します。
- crypto 関連のテスト (`cryptoTest`、`randomTest`) は、`LIBS += mock_openssl` だけを指定します。テストの `EXPECT_CALL` と `ON_CALL` の引数は変わりません。
- `docs/link-policy.md` の OpenSSL の記載を、動的リンクと同梱の内容へ書き換えます。

> [!IMPORTANT]
> `libcplat.so` を配布する場合は、`libcrypto_app.so.3` を同じディレクトリへ同梱する必要があります。  
> 同梱する場合の OpenSSL のライセンス条件 (Apache License 2.0) は、これまでの静的リンクの場合と同じです。

## 対象外

- zlib、cjson、lua、sqlite のシンボル名の衝突への対策 (makefw への SONAME とバージョン スクリプトの追加)。残課題一覧に記録します。
- libssl の mock。現在、libssl を利用している app はありません。
- zlib、cjson、lua、sqlite の mock への呼び出し元情報 (先頭 3 引数) の追加。残課題一覧に記録します。

## 変更対象

| 段階 | 対象 | 内容 |
| --- | --- | --- |
| 1 | `app/zlib`、`app/cjson`、`app/lua`、`app/sqlite` | `TARGET` の指定、mock の定数、公開シンボル検査、リンク指定、README.md、AGENTS.md |
| 1 | `app/cplat`、`app/porter`、`app/struct-meta` | リンク指定、`runtime-bundle.mk`、`link-policy.md` などの記載 |
| 2 | `app/openssl` | `build_openssl.py`、`test_build_openssl.py`、`mock_openssl.h`、差し替えヘッダー、mock 関数の実装、README.md、AGENTS.md |
| 2 | `app/cplat` | OpenSSL のリンク指定、`exports.map` のコメント、`runtime-bundle.mk`、crypto 関連テスト、`link-policy.md` |
| 1、2 | `app/general/docs` | 下記「正本ドキュメントへの反映」 |

Table: 変更対象

変更は複数のリポジトリ (サブモジュール) にまたがります。  
ステージングとコミットは、ユーザーの指示を受けてから行います。

## 正本ドキュメントへの反映

| 文書 | 反映する内容 |
| --- | --- |
| `app/general/docs/shared-library-mock-guideline.md` | 第三者管理ライブラリの命名規則 (接尾辞 `_app`) |
| `app/openssl/README.md` | 共有ライブラリの名前、`shlib_variant` を使う理由、リンク指定、mock の方式 (mock だけのリンク、実行時の実委譲、両 OS 対応)、置き換える関数の一覧と追加手順 |
| `app/zlib`、`app/cjson`、`app/lua`、`app/sqlite` の README.md | 新しいライブラリ名とリンク指定 |
| `app/cplat/docs/link-policy.md` | OpenSSL を動的にリンクすることと、同梱するファイル |
| `app/openssl/bin_internal/build_openssl.py` のコメント | `shlib_variant` の根拠 (`Configurations/README.md` の URL を `see:` で記載) と、リンク用ファイルの名前を変える理由 |

Table: 正本ドキュメントへの反映

命名規則の置き場所は、作業時に `app/general/docs` の文書一覧を確認し、より適切な文書があればそちらへ記載します。

## 受入条件

- Linux で、対象の 5 つの app と利用側 app (cplat、porter、struct-meta) のビルドとテストが成功する。
- ビルド後の `.warn` ファイルに、今回の変更に起因する警告がない。
- 第三者管理ライブラリの成果物が、「命名規則」の表の名前で生成される。変更前の名前の成果物と参照が残っていない。
- Linux の `libcrypto_app.so.3` の SONAME とシンボル バージョンが、この仕様のとおりである。
- `libcplat.so` が `libcrypto_app.so.3` に依存し、OS の libcrypto に依存していない。
- cplat の crypto 関連テストが、mock だけをリンクした状態で成功する (実委譲と、失敗の注入の両方)。mock 関数が呼び出し元のファイル名、行番号、関数名を受け取る。
- 1 つのソースが `<openssl/evp.h>`、`<openssl/rand.h>` をどちらの順で取り込んでも、差し替えヘッダーを通してコンパイルできる。
- `python3 -X warn_default_encoding -W error::EncodingWarning app/openssl/bin_test/test_build_openssl.py` が成功する。
- 変更した Markdown が、`text_style_jp.py` の確認で指摘なしである。
- Windows は CI の結果で確認する。CI に必要なコミットとプッシュは、ユーザーの指示を受けてから行う。

## 制約と未確認の事項

- OS 側が OpenSSL 3.x の環境で、同じプロセスに両方の OpenSSL が載る場合の動作は、開発環境 (OS 側は 1.1.1) では確認できていません。シンボル バージョンが異なるため混ざらない設計ですが、実機での確認は CI のコンテナーの版に依存します。
- OpenSSL の Windows の共有ビルドは、CRT を `/MD` でリンクします。Windows で OpenSSL を使う app を追加する場合は、利用側の CRT の設定と合わせる必要があります。現在、Windows で OpenSSL をリンクする app はありません。
- OpenSSL の版を上げて共有ライブラリの版の番号 (`.3`) が変わる場合は、配置物の名前と mock の定数を合わせて変更します。
