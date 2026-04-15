# potrConnectThread.c コールドアイレビュー

本ドキュメントは、`app\porter\prod\libsrc\porter\thread\potrConnectThread.c` の
`start_connected_threads()` を対象に、**現状のコードの趣旨と動作**を前提として整理した
レビュー結果です。過去の変更経緯やレビュー履歴ではなく、現在の実装から読み取れる責務と
整合性に焦点を当てます。

## 結論

`start_connected_threads()` の責務は、接続確立後に必要な依存スレッド
(`send`, `recv`, `health`) を起動し、途中で失敗した場合は**この関数が起動したものを
適切に巻き戻して `POTR_ERROR` を返すこと**です。

現状コードはその意図にほぼ沿っていますが、`tcp_recv_thread_start()` 失敗時の cleanup 条件
である L333 の `!ctx->send_thread_running` は、その意図と整合していません。

```c
if ((is_sender || is_bidir) && path_idx == 0 && !ctx->send_thread_running)
{
    close_tcp_conn(ctx, path_idx);
    potr_send_thread_stop(ctx);
}
```

この条件だと、**send スレッドが動いていないときに stop しようとする**形になっており、
rollback 判定として不自然です。レビュー観点では、ここは
`send_thread_running` の真偽を見るのではなく、**この呼び出しで send スレッドを起動したか**
で判断すべきです。

## 対象コードの意図

`start_connected_threads()` の冒頭では、以下の条件で send スレッドを起動します。

- `ctx->role == POTR_ROLE_SENDER` または `ctx->service.type == POTR_TYPE_TCP_BIDIR`
- `path_idx == 0`
- `!ctx->send_thread_running`

これは、**send スレッドは path ごとではなく共有で 1 本だけ持つ**という設計を示しています。
一方で `recv` と `health` は path ごとに起動されます。

したがって、この関数の中で起こり得る失敗は次の 3 段階です。

1. send スレッド起動失敗
2. recv スレッド起動失敗
3. health スレッド起動失敗

このとき期待される振る舞いは、**その時点までにこの関数が起動したリソースだけを片付ける**
ことです。

## 現状コードの動作整理

### 1. send スレッド起動失敗

`potr_send_thread_start()` が失敗した場合は、その場で `POTR_ERROR` を返しています。
この経路は自然です。`potr_send_thread_start()` 側で `send_thread_running` の更新と失敗時の
差し戻しを行っているため、`start_connected_threads()` 側で追加 cleanup は不要です。

### 2. recv スレッド起動失敗

問題はこの経路です。

send スレッドを先に起動した後で `tcp_recv_thread_start()` が失敗した場合、
`start_connected_threads()` の責務としては、**自分で起動した send スレッドを止めてから**
失敗を返すのが自然です。

ところが現状は、L333 で `!ctx->send_thread_running` を条件にしているため、
send スレッドを起動済みのケースで cleanup 条件が成立しにくくなっています。

少なくとも、関数の意図を素直に読む限り、ここで確認したいのは
「今 send スレッドが動作中か」ではなく、
「この `start_connected_threads()` 呼び出しが send スレッドを起動したか」です。

### 3. health スレッド起動失敗

`potr_tcp_health_thread_start()` 失敗時は、

1. `ctx->running[path_idx] = 0`
2. `close_tcp_conn(ctx, path_idx)`
3. `join_recv_thread(ctx, path_idx)`

まで行っています。これは **recv スレッドを自然終了させて join する** という意味で妥当です。

ただし、この経路でも直前に send スレッドを新規起動していたなら、設計の筋としては
recv 失敗時と同様に rollback 対象に含める方が一貫しています。

## なぜ `send_thread_running` だけでは不十分か

`ctx->send_thread_running` は、`PotrContext_` 全体で共有される
**send スレッドの現在状態**を表すフラグです。

しかし cleanup で必要なのは、共有状態そのものではなく、
**この関数呼び出しがその共有 send スレッドの生成に関与したか**という情報です。

この 2 つは意味が異なります。

| 観点 | 意味 |
|---|---|
| `ctx->send_thread_running` | 共有 send スレッドが今動いているか |
| rollback で本当に欲しい情報 | この関数呼び出しが send スレッドを起動したか |

そのため、L333 の条件を単純に `ctx->send_thread_running` に変えるだけでも不十分です。
それだと、**以前から動いていた共有 send スレッド**まで今回の失敗で止めてしまう可能性が
残ります。

## レビュー結果

コールドアイレビューとしての指摘は次のとおりです。

1. `is_bidir` / `is_sender` の宣言は妥当  
   これらは send スレッドの起動条件と cleanup 側条件の両方で使われており、
   現状コードでも役割があります。

2. L333 の `!ctx->send_thread_running` は rollback 条件として不整合  
   現状コードの意図に照らすと、ここは「send スレッドが未起動か」ではなく
   「この関数で send スレッドを起動したか」を見るべきです。

3. 修正方針は「条件反転」ではなく「局所状態の明示化」が適切  
   `started_send_thread` のような局所フラグを導入し、この呼び出しで起動した場合だけ
   rollback するのが最も読みやすく、安全です。

## 推奨修正方針

`start_connected_threads()` のローカル変数として
`int started_send_thread = 0;` を持ち、
`potr_send_thread_start()` 成功時に 1 を立てます。

その上で、`recv` / `health` 起動失敗時の cleanup は次の方針に揃えます。

- `started_send_thread == 1` のときだけ `potr_send_thread_stop(ctx)` を呼ぶ
- 既存の共有 send スレッドを誤って巻き込まない
- recv 失敗時と health 失敗時で rollback の考え方を揃える

擬似コードは以下です。

```c
int started_send_thread = 0;

if ((is_sender || is_bidir) && path_idx == 0 && !ctx->send_thread_running)
{
    if (potr_send_thread_start(ctx) != POTR_SUCCESS)
    {
        return POTR_ERROR;
    }
    started_send_thread = 1;
}

if (tcp_recv_thread_start(ctx, path_idx) != POTR_SUCCESS)
{
    close_tcp_conn(ctx, path_idx);
    if (started_send_thread)
    {
        potr_send_thread_stop(ctx);
    }
    return POTR_ERROR;
}

if (potr_tcp_health_thread_start(ctx, path_idx) != POTR_SUCCESS)
{
    ctx->running[path_idx] = 0;
    close_tcp_conn(ctx, path_idx);
    join_recv_thread(ctx, path_idx);
    if (started_send_thread)
    {
        potr_send_thread_stop(ctx);
    }
    return POTR_ERROR;
}
```

## 総評

現状コードの方向性自体は明確です。send は共有、recv/health は path ごと、失敗時は
起動済み依存スレッドを巻き戻す、という構造は読み取れます。

その中で L333 だけが rollback の責務表現としてずれているため、修正するなら
`send_thread_running` の真偽に依存するのではなく、**この呼び出しで起動したか**
という局所情報で cleanup を制御するのが妥当です。
