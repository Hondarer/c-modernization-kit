# Windows 用コンソールヘルパー設計ドキュメント

## 目的

- stdout / stderr をライブラリ内で差し替え、Windows コンソール（TTY）に対しては UTF-16（`WriteConsoleW`）で出力し、パイプやファイルへは UTF-8 バイト列をそのまま書き戻す。
- stdin は差し替えない。UTF-8 マニフェスト（`activeCodePage` = UTF-8）により、CRT の入力関数（`fgets` / `scanf` / `getchar`）がコンソールから UTF-8 をネイティブに読み取れるため、ライブラリの介入は不要とする。

---

## 前提

Linux / Windows 双方で利用可能な文字コード安全なコンソールアプリケーションを作成するにあたり、
Windows では、マニフェストによるコードページの固定だけでは、コンソール出力やパイプ送受信で文字化けの懸念がある。

Windows では、stdout / stderr をフックすることで、コンソール出力やパイプ送受信でユーザーに意識させずに
正しく UTF-8 で入出力できるようにする。

アプリケーションは、Windows の場合初期化関数をコールする必要があるが、
コール後は、Linux と同じコードで文字コード安全なコンソールアプリケーションとすることができる。

---

## 既存プロジェクトの対応方式

本ライブラリの設計にあたり、Git / Rust / Node.js (libuv) の Windows コンソール UTF-8 対応方式を調査した。

### 出力方向（stdout / stderr）

| プロジェクト | 方式 | 出力 API | パイプ差し替え |
|------------|------|---------|--------------|
| **Git** (`compat/winansi.c`) | パイプ差し替え + 専用スレッド + ANSI エスケープ処理 | `WriteConsoleW` | する |
| **Rust** (`library/std/src/sys/stdio/windows.rs`) | CRT バイパス、直接 Windows API を使用 | `WriteConsoleW` | しない |
| **Node.js** (`deps/uv/src/win/tty.c`) | CRT バイパス、直接 Windows API を使用 | `WriteConsoleW` | しない |

- 全プロジェクトが `WriteConsoleW` を使用。
- Git のみパイプ差し替え方式を採用しており、本ライブラリと同様のアプローチである。Rust / Node.js は CRT を使わない独自 I/O レイヤーを提供するため、パイプ差し替えは不要。
- 本ライブラリは CRT の `printf` / `fprintf` をそのまま使う設計であるため、Git と同じパイプ差し替え方式が適切である。

### 入力方向（stdin）

| プロジェクト | 方式 | 通常モード API | Raw モード API | stdin パイプ差し替え | 独自 Line Discipline |
|------------|------|-------------|-------------|-------------------|---------------------|
| **Git** | stdin に一切触れない | CRT 標準 | N/A | しない | 不要 |
| **Rust** | CRT バイパス、直接 API | `ReadConsoleW` | N/A | しない | 不要 |
| **Node.js** | CRT バイパス、モード切替 | `ReadConsoleW` | `ReadConsoleInputW` | しない | 不要 |

- 3 プロジェクトいずれも stdin をパイプに差し替えていない。
- 独自の行規律（Line Discipline）を実装しているプロジェクトもない。コンソールの `ENABLE_LINE_INPUT` による行編集機能をそのまま利用する。
- Git は stdin に一切介入しない（最もシンプル）。Rust は `ReadConsoleW` を直接使用するが CRT をバイパスする独自 I/O のため。Node.js は通常モード（`ReadConsoleW`）と Raw モード（`ReadConsoleInputW`）を `SetConsoleMode` で切り替える。

### 本ライブラリへの適用

- **出力**: Git と同じパイプ差し替え + `WriteConsoleW` 方式を採用する。CRT の `printf` / `fprintf` をそのまま使うため最適。
- **入力**: Git と同じく stdin に介入しない方式を採用する。UTF-8 マニフェストにより CRT の入力関数が UTF-8 をネイティブに扱えるため、パイプ差し替えや独自行規律は不要。

---

## 設計概要

本ライブラリの内部スレッドは、stdout / stderr について**文字（バイト）単位で転送**する。
出力方向では到着したデータを即座に転送し、ライブラリ側で行バッファリングは行わない。
入力方向（stdin）にはライブラリは介入しない。コンソールの行編集機能（Backspace、カーソル移動、
コマンド履歴等）はコンソールがネイティブに提供し、UTF-8 マニフェストにより
CRT の入力関数が UTF-8 を直接読み取る。
ライブラリはエンコーディング変換（UTF-16 → UTF-8、出力方向のみ）と透過的な転送を担い、
既存のプログラムを一切変更せず動作させることを目指す。

### 1. ライブラリ API

- `int windows_console_init(void);`
  - 戻り値 0 成功、非ゼロで失敗。パイプの作成、既存 stdout / stderr の複製、差し替え、バックグラウンドスレッドの起動を行う。stdin には触れない。
- `void windows_console_dispose(void);`
  - バックグラウンドスレッドを停止し、ハンドル / FD を元に戻す。
- 追加オプション: `windows_console_set_flags(uint32_t flags)` などで挙動（stdout / stderr の個別有効化等）を変更可能にする。

### 2. 出力方向の差し替えの流れ（stdout / stderr）

- 起動時:
  1. `GetStdHandle(STD_OUTPUT_HANDLE / STD_ERROR_HANDLE)` で元ハンドルを取得し、`DuplicateHandle` で安全に保存する。
  2. `CreatePipe` で匿名パイプ（read, write）を作成。
  3. `_open_osfhandle` を使い write 側ハンドルから CRT のファイルディスクリプタを作成し、`_dup2` で stdout / stderr に割り当てる。
  4. 読み取り側ハンドルは別スレッドで待ち受ける。

- 読み取りスレッド:
  1. `ReadFile` / `ReadFileEx` で UTF-8 バイト列を受け取る。
  2. 出力先の判定（元ハンドル）:
     - `GetFileType` + `GetConsoleMode` を組み合わせてコンソールかを判定。
     - コンソールなら `MultiByteToWideChar(CP_UTF8, ...)` で UTF-16 に変換して `WriteConsoleW`。
     - 非コンソールなら `WriteFile(元ハンドル, buffer, size, ...)` で UTF-8 バイト列をそのまま書く。
  3. 到着したデータは即座に転送する（行単位のバッファリングは行わない）。コンソール出力時に UTF-8 マルチバイトシーケンスが不完全な状態で分割されていた場合のみ、不完全な末尾バイトを次回の読み取りまで保留する。バイナリデータ（NUL を含むなど）にも耐える実装とする。

### 3. 入力方向（stdin）

stdin はライブラリの差し替え対象外とする。

- **コンソール入力**: UTF-8 マニフェスト（Windows 10 1903+ の `activeCodePage` = UTF-8）により、CRT の入力関数（`fgets` / `scanf` / `getchar` 等）がコンソールから UTF-8 をネイティブに読み取る。`ENABLE_LINE_INPUT` によるコンソールの行編集機能（Backspace、カーソル移動、コマンド履歴等）もそのまま動作する。
- **パイプ入力**: パイプからの入力は生のバイト列（UTF-8 前提）であり、CRT がそのまま読み取る。変換は不要。
- **`SetConsoleMode`**: stdin はコンソールハンドルのままであるため、アプリケーションから `SetConsoleMode` で `ENABLE_LINE_INPUT` / `ENABLE_ECHO_INPUT` 等のフラグを自由に変更できる。
- **`_getch()` / `_getwch()`**: stdin を差し替えないため、CRT の `_getch()` / `_getwch()` も通常どおり動作する。入力バッファの競合は発生しない。

stdin を差し替えない設計とした理由:

1. **主要プロジェクトとの整合性**: Git / Rust / Node.js いずれも stdin をパイプに差し替えていない。
2. **行編集の維持**: stdin をパイプに差し替えると、コンソールの行エディタ（`ENABLE_LINE_INPUT`）がバイパスされ、Backspace やカーソル移動が動作しなくなる。独自の行規律（Line Discipline）を実装する必要が生じるが、コンソール固有機能（doskey 連携、マウス選択等）の完全な再現は困難である。
3. **透過性**: `SetConsoleMode` / `_getch()` がそのまま動作するため、既存プログラムの変更が不要。
4. **UTF-8 マニフェストの活用**: Windows 10 1903+ では UTF-8 マニフェストにより CRT の入力関数が UTF-8 をネイティブに扱えるため、エンコーディング変換のためのフックは不要。

### 4. stderr の扱い

stderr も stdout と同様に独立した pipe / thread を用意する。

### 5. エラー処理

- `WriteConsoleW` が失敗した場合は、元ハンドルへ UTF-8 バイト列を戻すフォールバックを行う。
- ライブラリ初期化失敗時は呼び出し元が通常の stdout / stderr を使用するようにする。

### 6. 互換性と注意点

- CRT の `setvbuf` / `_setmode` の設定がある場合、差し替え後も影響を受ける可能性がある。ドキュメントで既知の影響を明記する。
- Windows Terminal や ConHost、パイプ先のツールの挙動差に留意する。
- `SetConsoleCP(CP_UTF8)` / `SetConsoleOutputCP(CP_UTF8)` によるコードページ変更は Windows 10 1607 以降で利用できる簡易的な代替手段だが、パイプ経由の出力には効果がなく、本ライブラリは出力方向についてコードページ設定に依存しない方式を採用する。
- 入力方向は UTF-8 マニフェストに依存する。Windows 10 1903 未満の環境では UTF-8 マニフェストが動作しないため、CRT の入力関数がシステムコードページでの読み取りとなる。この場合は `SetConsoleCP(CP_UTF8)` を呼び出す等のアプリケーション側の対応が必要であり、本ライブラリのスコープ外とする。

---

## テスト計画

- 自動テスト（出力）: パイプを受けるテストプロセスを用意し、出力が UTF-8 のまま転送されること、またコンソールでは日本語・絵文字が正しく表示されることを検証する。
- 自動テスト（入力）: UTF-8 マニフェストが有効な状態で、UTF-8 文字列をパイプ経由で標準入力に与え、アプリケーションが正しく読み取れることを検証する。コンソール入力はライブラリの差し替え対象外のため、マニフェスト自体の動作確認として位置付ける。
- カバレッジ: 失敗パス（`WriteConsoleW` 失敗、`ReadFile` 中断）もテストする。

---

## 実装ファイル

| ファイル | 役割 |
|---------|------|
| `prod/util/include/windows_console.h` | 公開ヘッダー（API 宣言） |
| `prod/util/libsrc/windows_console/windows_console.c` | コア実装（stdout / stderr 差し替え＋スレッド） |
| `prod/util/libsrc/windows_console/makefile` | ビルド設定（既存の makepart.mk に統合） |
| `prod/util/docs/windows_console.md` | このファイル |
| `test/util/src/windows_consoleTest/*` | テストコード |

---

## 開発の順序

1. `windows_console.h` の API 定義と設計確定
2. `windows_console.c` のコア実装（stdout / stderr 差し替え＋読み取りスレッド）
3. makefile 統合・ビルド確認
4. テスト作成と CI での検証
5. ドキュメントの整備とサンプルの追加

---

## コード例

マニフェストは `makefw/docs-src/windows-utf8-console.md` によって埋め込まれている前提。

```c
#include "windows_console.h"

int main(void) {
    windows_console_init();        /* stdout / stderr を差し替え */
    printf("こんにちは\n");
    char buf[256];
    fgets(buf, sizeof(buf), stdin); /* UTF-8 マニフェストにより UTF-8 で読み取れる */
    windows_console_dispose();
    return 0;
}
```
