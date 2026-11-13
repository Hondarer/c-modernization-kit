<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: grok / セッション ID: 01a1d14c-c0a7-7ee0-9b1b-3b35f72d7126
検討 ID: source-comment-bl029-20261113
合意版: 6
上流文書: `261113-source-comment-bl029-discussion.md` (検討用 Markdown)。削除されている場合があります。
下流文書: `261113-source-comment-bl029-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# BL-029 の規範との差 4 件 最終仕様

ファイル名: `261113-source-comment-bl029-design.md`

## 目的

BL-029 に残っている規範との差を、現行のソースと規範に合わせて解消します。  
あわせて、単文の制御文にも波括弧を必須とする規則を共通コーディング規範へ追加し、第一者管理の C / C++ をその形へそろえます。

対象は次の 4 系統です。

- 制御文の本体の波括弧
- `bench-tracer` のプロセス数とメッセージ数の積
- `cplat_mmap_attach` の呼び出し例
- `clock_failure_preserves_timestamp` の確認タグ

## 制御文の波括弧

`if`、`else if`、`else`、`for`、`while`、`do` の本体は、文が 1 つでも、空でも、波括弧で囲みます。  
`else if` では、`else` と `if` の間に波括弧を置かず、続く `if` の本体を波括弧で囲みます。

> [!NOTE]
> 波括弧の追加は、`continue`、`break`、`return` の飛び先を変えません。  
> `else` が対応する `if` も変えません。  
> 内側の `if` に付いている `else` は、内側の本体を囲んだあとも同じ `if` に付けます。

### 追加する規範

`app/general/docs/coding-guideline.md` の「制御構造の制限」の末尾、三項演算子の節の直後に、次の節を追加します。  
本書冒頭の「適用範囲は主に `app/`」は、他の節の範囲として残します。  
この節だけが、下に書く第一者管理の範囲へ適用されます。

次の見出しから、検証の段落までを、共通コーディング規範へ移します。

### 制御文の本体は波括弧で囲む

`if`、`else if`、`else`、`for`、`while`、`do` の本体は、文が 1 つでも、空でも、波括弧で囲みます。  
`else if` では、`else` と `if` の間に波括弧を置きません。  
続く `if` の本体を波括弧で囲みます。  
マクロの置換結果も同じです。

新規のコードも、既存のコードも、この形にします。

この節は、このリポジトリで作成した C / C++ に適用します。  
`app/` と `framework/` を含みます。  
テスト、コマンド、サンプル、ヘッダー、マクロ本体を含みます。  
外部 OSS、第三者の配布物、ビルド生成物 (bison と flex の出力、`gen/` 配下) は含めません。

`switch` は、この節で必須にする対象キーワードにはしません。  
`switch` の中にある `if`、`for`、`while`、`do` は、この節に従います。

望ましい例は、波括弧を次の行に置きます。

```c
if (rc == 0)
{
    return CPLAT_OK;
}
else
{
    return rc;
}
```

空の本体は、波括弧だけにします。  
単独のセミコロンは残しません。

```c
while (waitpid(-1, NULL, WNOHANG) > 0)
{
}
```

望ましくない例は、本体が 1 文で波括弧がありません。

```c
if (rc == 0)
    return CPLAT_OK;
```

検証は、制御文の本体を目視して確認します。  
文字列探索は補助であり、複数行の条件やマクロの継続行は探索結果だけで判断しません。  
`.clang-format` の `InsertBraces` は true です。  
通常の `if`、`else`、`for`、`do` では、整形時に波括弧が入ります。  
本体が空の `while` と、マクロの継続行の中の制御文は、clang-format 22.1.4 では波括弧が入らないことがあります。

### .clang-format の InsertBraces

リポジトリ直下の `.clang-format` に、`BreakBeforeBraces` の直後へ次を追加します。  
他のキーは変えません。  
この設定ファイルは、リポジトリ内にこの 1 つです。

```yaml
# 制御文の本体へ波括弧を挿入する
# 本体が空の while と、マクロ継続行の中では挿入されないことがある (clang-format 22.1.4)
InsertBraces: true
```

`app/general/docs/source-style-guideline.md` の `.clang-format` の基本構文の列挙へ、同じ内容の 1 項目を追加します。  
通常の `if`、`else`、`for`、`do` は整形時に波括弧が入ること、空の `while` とマクロ継続行では clang-format 22.1.4 で入らないことがあることを書きます。

`InsertBraces: true` を書いたあと、既存の第一者管理の C / C++ を一括整形します。  
手順は次のとおりです。

1. 各 Git ルートの管理対象から、拡張子が `.c`、`.h`、`.cc`、`.cpp`、`.cxx`、`.hh`、`.hpp`、`.hxx` のファイルを列挙します。
2. シンボリック リンクを入力から除きます。テスト配下のリンク パスは、ここに含まれます。2026-11-13 の列挙では、この拡張子のリンクは 341 件です。
3. 後の節で第三者の配布物として挙げたパスと、生成物 (`gen/`、`obj/`、bison と flex の出力、`framework/docsfw/livedocs/.venv/`) を入力から除きます。
4. 残った実ファイルへ `clang-format -i` を実行します。リンク先が第一者管理の通常ファイルであるときは、その実パスがこの入力に一度だけ入ります。リンク先が生成物または第三者管理であるときは、実パスも入りません。
5. 差分を確認します。除外したパスが差分に無いこと、差分が字下げ、改行、波括弧の追加などの整形であることを見ます。処理の意味が変わる差分があれば、そのファイルで作業を止め、完了報告の前に内容を示します。
6. 確認結果を完了報告に含めます。含めるものは、入力件数、変更件数、除外の確認、作業を止めたファイルがあればその一覧です。

空の `while` とマクロ継続行は、この整形では波括弧が入らないことがあります。  
整形のあと、残っているものを手で直します。

> [!NOTE]
> `InsertBraces: true` は、通常の制御文へ波括弧を足します。  
> 付いている波括弧を外す設定ではありません。  
> `BreakBeforeBraces` の Allman は維持します。

### 今回直すコード

第一者管理の C / C++ を、今回まとめて直します。  
第一者管理とは、このリポジトリで作成したコードです。  
外部 OSS、第三者の配布物、ビルド生成物は含めません。

直す文は、本体の次のトークンが `{` でない `if`、`else`、`for`、`while`、`do` です。  
条件が複数行でも、本体に波括弧がある文は直しません。  
`do` のうしろに続く `while` は、ループの開始ではありません。

次のパスは、第三者の配布物または生成物として直しません。

- `app/lua/prod/`
- `app/sqlite/prod/`
- `app/zlib/prod/include/` と `app/zlib/prod/libsrc/`
- `app/openssl/prod/`
- `app/cjson/prod/include/cJSON.h`、`app/cjson/prod/include/cJSON_Utils.h`、`app/cjson/prod/libsrc/cjson/cJSON.c`、`app/cjson/prod/libsrc/cjson/cJSON_Utils.c`
- `framework/testfw/gtest/`
- `framework/docsfw/bin_internal/modules/`
- `framework/docsfw/styles/widdershins/`
- `framework/docsfw/livedocs/.venv/`
- `gen/`、`obj/`、bison と flex の出力

同じ app の中にある第一者のコードは対象です。  
cjson の整数拡張、JSONC、サンプル、各 app のテストと mock、OpenSSL のテスト用オーバーライド ヘッダーが該当します。  
2026-11-13 の探索では、これらのパスに波括弧のない制御文はありませんでした。  
実装の開始時に探索し直し、見つかった文は直します。

マクロは、継続行のバックスラッシュを残したまま、本体を波括弧で囲みます。  
すでに `{` がある `do` `while (0)` は対象外です。

シンボリック リンクは、リンク先の実体を 1 回直します。  
リンク側のパスは編集しません。  
次のテスト用 `.c` は、いずれも `app/porter/prod/` 配下へのリンクです。

- `app/porter/test/src/libporter/apiOpenPathsTcpTest/api_open_paths_tcp.c`
- `app/porter/test/src/libporter/apiOpenPathsUdpTest/api_open_paths_udp.c`
- `app/porter/test/src/libporter/potrHealthThreadTest/potr_health_thread.c`
- `app/porter/test/src/libporter/potrSendThreadTest/potr_send_thread.c`
- `app/porter/test/src/libporter/potrServiceCloseTest/potr_service_close.c`
- `app/porter/test/src/libporter/threadConnectPrepareTest/thread_connect_prepare.c`
- `app/porter/test/src/libporter/threadRecvHealthTest/thread_recv_health.c`
- `app/porter/test/src/libporter/threadRecvValidateTest/thread_recv_validate.c`
- `app/porter/test/src/libporter/threadRecvWindowTest/thread_recv_health.c`

### 候補ファイル

2026-11-13 の文単位の探索で、波括弧のない制御文があった実ファイルです。  
件数は候補です。  
編集前に各文を読み、誤検出は直さず、探索から漏れた第一者管理の文は追加で直します。

| ファイル | 候補数 |
| --- | --- |
| `app/porter/prod/libsrc/porter/api/api_open_paths_tcp.c` | 2 |
| `app/porter/prod/libsrc/porter/api/api_open_paths_udp.c` | 5 |
| `app/porter/prod/libsrc/porter/api/potr_service_close.c` | 4 |
| `app/porter/prod/libsrc/porter/thread/potr_health_thread.c` | 5 |
| `app/porter/prod/libsrc/porter/thread/potr_send_thread.c` | 5 |
| `app/porter/prod/libsrc/porter/thread/thread_connect_prepare.c` | 12 |
| `app/porter/prod/libsrc/porter/thread/thread_recv_health.c` | 9 |
| `app/porter/prod/libsrc/porter/thread/thread_recv_validate.c` | 4 |
| `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_linux.c` | 1 |
| `app/cplat/test/src/libcplatTest/prompt/promptEditTest/promptEditTest.cc` | 2 |
| `app/cplat/test/src/libcplatTest/prompt/promptTest/promptTest.cc` | 1 |
| `app/cplat/test/src/libcplatTest/trace/trace_syslogTest/trace_syslogTest.cc` | 5 |
| `app/porter/test/src/cmd/porterTestOutputTest/porterTestOutputTest.cc` | 1 |
| `app/porter/test/src/integration/porterSendRecvTest/handleConcurrencyTest.cc` | 4 |
| `app/porter/test/src/integration/porterSendRecvTest/tcpMultipathTest.cc` | 3 |
| `app/porter/test/src/integration/tcpServerPreforkTest/tcpServerPreforkTest.cc` | 22 |
| `app/porter/test/src/libporter/potrConnectThreadTest/potrConnectThreadTest.cc` | 4 |
| `app/porter/test/src/libporter/potrHandleTest/potrHandleTest.cc` | 2 |
| `app/porter/test/src/libporter/potrNotifyTest/potrNotifyTest.cc` | 2 |
| `app/porter/test/src/libporter/potrSendQueueTest/potrSendQueueInitTest.cc` | 16 |
| `app/porter/test/src/libporter/potrServiceOpenTest/potrServiceOpenTest.cc` | 154 |
| `app/porter/test/src/libporter/threadConnectPrepareTest/threadConnectPrepareTest.cc` | 7 |
| `framework/testfw/include/mock_factory_head.h` | 11 |
| `framework/testfw/libsrc/mock_libssh/mock_sftp.cc` | 7 |

Table: 波括弧を追加する候補ファイル

`potr_send_thread.c` の候補 5 件は、`tcp_write_all_paths` の 2 件、`flush_packed_peer` の 1 件、`send_packed_peer_mode` の 2 件です。  
`tcpServer_linux.c` の 1 件は、本体が空の `while` です。  
`mock_factory_head.h` の 11 件は、void 戻り値の mock マクロの中の `if` です。

## bench-tracer の積

`app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c` は、プロセス数とメッセージ数がどちらも `int` に収まっていても、積が `int` を超えることがあります。  
その積は、`main` の `measure_filtered(messages * processes)` と、`measure_files` の `printf` に渡す `processes * messages` で符号付きの乗算になります。

0 以下を拒否した直後、両方の呼び出しより前に、積が `int` の最大値を超える入力を拒否します。  
上限は `int` の最大値 (`INT_MAX`) です。  
これより小さい上限は設けません。  
プロセス数もメッセージ数も、積とは別の個別上限は設けません。  
この扱いは、現状の起動のまま完了とします。  
残課題には残しません。

プロセス数は、この検査の前に 1 以上であることが決まっています。  
検査は乗算の前に行い、除算は `messages > INT_MAX / processes` とします。  
`limits.h` が未インクルードなら追加します。

拒否時は終了値 1 で戻り、標準エラー出力へ次の 2 行を書きます。

```text
bench-tracer: product of processes and messages does not fit in int
usage: bench-tracer [path [processes [messages]]]
```

既定値の 16 と 1000 の積は、この検査を通過します。  
`measure_files` の中にある、乗算の前に `uint64_t` へ広げる式は変更しません。

`app/cplat/test/src/cmd/bench-tracerTest/bench-tracerTest.cc` に、拒否を確認するテストを追加します。  
既存の `rejects_invalid_numeric_arguments` は変更しません。  
入力は、プロセス数とメッセージ数がどちらも `int` に収まり、積だけが `int` の最大値を超える組です。  
終了値は 1 です。  
測定用 tracer の生成は 0 回です。  
成功する経路で、上限いっぱいの回数を反復するテストは追加しません。  
追加したテストのエビデンスは、テスト実行で生成します。

> [!NOTE]
> メッセージ数が 1 のとき、プロセス数が `int` の最大値でも積は `int` に収まります。  
> その入力は子プロセスの起動へ進みます。  
> プロセス数の資源上の上限は設けず、この起動のままとします。

## mmap の呼び出し例

`app/cplat/docs/coding-guideline.md` の判定慣用句にある `cplat_mmap_attach` の例を、公開宣言と同じ 5 引数にします。  
第 5 引数は `NULL` です。  
`NULL` のときは、エラーの詳細を返しません。

```c
ret = cplat_mmap_attach(path, access, create_size, &map_out, NULL);
```

この節の主題は、結果コードを `CPLAT_OK` と比較することのままです。  
同じファイル内の他の文は変更しません。

## 確認タグ

`app/cplat/test/src/libcplatTest/trace/traceCommonTest/traceCommonTest.cc` の `clock_failure_preserves_timestamp` にある、タグのないナノ秒の比較へ、次の確認タグを付けます。

```cpp
EXPECT_EQ(456, resolved.tv_nsec); // [確認_異常系] - resolved.tv_nsec が 456 であること。(取得失敗前のナノ秒を保持することの確認)
```

同じテストの他のコメントは変更しません。  
このテスト名の `results.md` は、テスト ディレクトリ配下にはありません。  
テストを実行した場合に、新しいエビデンスが生成されます。

## 知見の反映

永続的に残す内容は、上の「追加する規範」の節と、`.clang-format` の `InsertBraces`、ソース スタイル規範の対応する 1 項目です。  
波括弧を足した個々の箇所へ、今回の修正である旨のコメントは置きません。  
`bench-tracer` の拒否は、コマンドの既存の失敗と同じ終了値 1 であり、終了コードの新しい区分は作りません。

## 受入条件

- 追加した節が、上の規則と適用範囲を共通コーディング規範に書いている。
- 候補ファイルの波括弧のない対象制御文が、目視のあとで波括弧付きになっている。
- リポジトリ直下の `.clang-format` に `InsertBraces: true` があり、他のキーは変わっていない。
- ソース スタイル規範に、その設定と、空の `while` およびマクロ継続行の限界が書いてある。
- `InsertBraces: true` のあと、第一者管理の C / C++ 実ファイルを一括整形している。
- 第三者管理ソース、生成物、テストのシンボリック リンク パスは、整形の入力に無く、差分にも無い。
- 一括整形の入力件数、変更件数、除外の確認を完了報告に含めている。
- 処理の意味が変わる差分は残していない。
- `bench-tracer` が、積が `int` に収まらない入力を終了値 1 と上の 2 行で拒否する。
- プロセス数とメッセージ数の個別上限は追加していない。
- 新しい拒否テストが、終了値 1 と tracer 生成 0 回を確認する。
- `cplat_mmap_attach` の例が 5 引数になり、第 5 引数が `NULL` である。
- ナノ秒の `EXPECT_EQ` に、上の確認タグが 1 行付いている。
- 変更した app と `framework/testfw` について、変更ファイルをコンパイルする既存のテストを Linux で実行し、成功している。
- ビルド後、対象範囲の `.warn` を確認している。
- 変更した Markdown に `text_style_jp.py` を実行し、不自然な変換がない。
- CI は起動しない。
- Windows での実行は、この受入条件に含まない。

## 今回やらないこと

- プロセス数とメッセージ数の、積とは別の個別上限。現状の起動のまま完了とし、残課題には残さない
- 第三者管理ソース、生成物、テストのシンボリック リンク パスの整形
- `switch` 自体への波括弧の必須化
- 過去の残課題のうち、確認できない設計理由の一覧、Windows 実機の確認、TCP 双方向 N:1 通信
