Windows 用コンソールヘルパー設計ドキュメント

目的
- stdout/stderr をライブラリ内で差し替え、Windows コンソール（TTY）に対しては UTF-16（WriteConsoleW）で出力し、パイプやファイルへは UTF-8 バイト列をそのまま書き戻す。

設計概要
1. ライブラリ API
   - int util_console_init(void);
     - 戻り値 0 成功、非ゼロで失敗。パイプの作成、既存 stdout/stderr の複製、差し替え、バックグラウンド読み取りスレッドの起動を行う。
   - void util_console_dispose(void);
     - バックグラウンドスレッドを停止し、ハンドル/FD を元に戻す。
   - 追加オプション: util_console_set_flags(uint32_t flags) などで挙動（stdout/stderr の個別有効化、行バッファ制御）を変更可能にする。

2. 差し替えの流れ
   - 起動時:
     1. GetStdHandle(STD_OUTPUT_HANDLE/STD_ERROR_HANDLE) で元ハンドルを取得し、DuplicateHandle で安全に保存する。
     2. CreatePipe で匿名パイプ（read, write）を作成。
     3. _open_osfhandle を使い write 側ハンドルから CRT のファイルディスクリプタを作成し、_dup2 で stdout/stderr に割り当てる。
     4. 読み取り側ハンドルは別スレッドで待ち受ける。

   - 読み取りスレッド:
     1. ReadFile/ReadFileEx で UTF-8 バイト列を受け取りバッファに蓄積。
     2. 出力先の判定（元ハンドル）:
        - GetFileType + GetConsoleMode を組み合わせてコンソールかを判定。
        - コンソールなら MultiByteToWideChar(CP_UTF8, ...) で UTF-16 に変換して WriteConsoleW。
        - 非コンソールなら WriteFile(元ハンドル, buffer, size, ...) で UTF-8 バイト列をそのまま書く。
     3. 行バッファ処理: 行単位またはタイムアウトでフラッシュする。バイナリデータ（NUL を含むなど）にも耐える実装とする。

3. stderr の扱い
   - stderr も同様に独立した pipe/thread を用意する。必要に応じて両方の出力を同一スレッドで扱う設計も検討するが、シンプルさ優先で独立実行を推奨。

4. エラー処理
   - WriteConsoleW が失敗した場合は、元ハンドルへ UTF-8 バイト列を戻すフォールバックを行う。
   - ライブラリ初期化失敗時は呼び出し元が通常の stdout/stderr を使用するようにする。

5. 互換性と注意点
   - CRT の setvbuf/_setmode の設定がある場合、差し替え後も影響を受ける可能性がある。ドキュメントで既知の影響を明記する。
   - Windows Terminal や ConHost、パイプ先のツールの挙動差に留意する。

6. テスト計画
   - 自動テスト: パイプを受けるテストプロセスを用意し、出力が UTF-8 のまま転送されること、またコンソールでは日本語・絵文字が正しく表示されることを検証する。
   - カバレッジ: 失敗パス（WriteConsoleW 失敗、ReadFile 中断）もテストする。

7. 実装ファイル
   - prod/util/include/console.h
   - prod/util/libsrc/console/console.c
   - prod/util/libsrc/console/makefile (既存の makepart.mk に統合)
   - prod/util/docs/console.md (このファイル)
   - test/console/* (テストコード)

8. 開発の順序
   1. console.h の API 定義と設計確定
   2. console.c のコア実装（stdout 差し替え + 読み取りスレッド）
   3. makefile 統合・ビルド確認
   4. テスト作成と CI での検証
   5. ドキュメントの整備とサンプルの追加

付録: 簡易的な Read-Thread の疑似コード

// 省略: 実装時に console.c に詳細コードを追加

---

補足: マニフェスト設定時の printf の動作
--------------------------------------------

マニフェスト（`activeCodePage=UTF-8`）を設定済みのプログラムは、**追加の初期化なしに `printf` だけで UTF-8 コンソール出力が機能します**（Windows 10 1903 以降）。

### 仕組み

`activeCodePage=UTF-8` マニフェストを埋め込んで起動すると、Windows ローダーがプロセスの ACP (Active Code Page) を 65001 (UTF-8) に設定します。これに伴い、コンソール出力コードページ（`GetConsoleOutputCP()`）も自動的に 65001 に切り替わります。

その結果：

- `printf` が出力した UTF-8 バイト列を、コンソールが UTF-8 として正しく解釈・表示する
- `SetConsoleOutputCP(CP_UTF8)` の明示的な呼び出しは不要になる（重複呼び出しになるが副作用はない）
- `argv` のエンコーディングも UTF-8 になり、日本語ファイルパスなども正しく扱える

### `util_console_init` が依然として有効なシナリオ

マニフェストがあっても、以下の場面では `util_console_init`（WriteConsoleW 方式）が有効です。

| シナリオ | 理由 |
|:---------|:-----|
| Windows 10 1903 未満をサポートする | マニフェストの `activeCodePage` は 1903 未満では無視される。コンソール出力の保証には `SetConsoleOutputCP(CP_UTF8)` または `util_console_init` が必要 |
| stdout/stderr をパイプやファイルにリダイレクトする | CRT の `printf` はパイプへ UTF-8 バイト列をそのまま書くが、バッファリングや途中フラッシュで不完全なマルチバイト列が分断されることがある。`util_console_init` の読み取りスレッドは行単位でバッファリングしてこれを防ぐ |
| WriteConsoleW による確実な Unicode 出力が必要 | コンソール CP が 65001 でも、CRT 経由の `printf` と `WriteConsoleW` では内部パスが異なる。`util_console_init` は直接 `WriteConsoleW` を呼ぶため、環境依存の文字化けを排除できる |
| CRT バッファリング設定との干渉を避けたい | `setvbuf` や `_setmode` で独自設定している場合、マニフェスト方式だけでは出力の乱れが起きることがある |

### 推奨パターン

単純な UTF-8 テキスト出力のみのプログラムであれば、マニフェスト埋め込みと `printf` の組み合わせで十分です。

```c
/* マニフェスト (activeCodePage=UTF-8) を埋め込んでいる前提 */
/* Win10 1903 以降では printf だけで UTF-8 コンソール出力が機能する */
printf("こんにちは\n");
```

より広い互換性やパイプ出力の堅牢性が求められる場合は `util_console_init` を使用します。

```c
#include <console.h>

int main(void) {
    util_console_init();   /* stdout/stderr を差し替え、WriteConsoleW 経由に切り替える */
    printf("こんにちは\n");
    util_console_dispose();
    return 0;
}
```

詳細な比較（`SetConsoleOutputCP` / マニフェスト / 両方の使い分け）は  
`makefw/docs-src/windows-utf8-console.md` を参照してください。


