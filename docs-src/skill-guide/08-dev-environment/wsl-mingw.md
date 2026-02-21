# WSL / MinGW 環境

## 概要

Windows で Linux 向け C コードをビルドするには、Linux 互換の開発環境が必要です。主な選択肢として、**WSL**（Windows Subsystem for Linux）と **MinGW**（Minimalist GNU for Windows）があります。WSL は Windows 上で Linux カーネルを動作させる仕組みで、実際の Linux 環境とほぼ同じ開発体験が得られます。MinGW は Windows ネイティブで動作する GCC ツールチェインを提供します。

このリポジトリは Linux（GCC/WSL）と Windows（MSVC/MinGW）の両方をサポートしています。Windows での開発には、あらかじめインストールされているポータブル版 Visual Studio Build Tools（VSBT）と Git for Windows を使用します。`Start-VSCode-With-Env.ps1` スクリプトがこれらの環境変数を自動設定して VS Code を起動します。

WSL を使う場合は Linux 環境と同様の手順でビルドでき、MinGW（Git for Windows 付属）を使う場合は Windows ネイティブバイナリのビルドが可能です。

## 習得目標

- [ ] WSL のインストールと基本的な Linux コマンドの実行ができる
- [ ] WSL から VS Code を起動し、C コードを編集・ビルドできる
- [ ] Git for Windows の Git Bash でシェルコマンドを実行できる
- [ ] `Start-VSCode-With-Env.ps1` の役割と使い方を理解できる
- [ ] Windows と Linux での実行ファイル・ライブラリのファイル名の違いを説明できる
- [ ] `make` コマンドが Windows で動作するよう環境が設定されていることを確認できる

## 学習マテリアル

### 公式ドキュメント

- [WSL インストールガイド（Microsoft Learn）](https://learn.microsoft.com/ja-jp/windows/wsl/install) — WSL のインストール手順（日本語）
  - WSL 2 の有効化と Linux ディストリビューションのインストール
- [VS Code + WSL の設定](https://code.visualstudio.com/docs/remote/wsl) — VS Code Remote - WSL 拡張機能の使い方（英語）
- [Git for Windows](https://gitforwindows.org/) — Git for Windows の公式サイト（英語）

## このリポジトリとの関連

### 使用箇所（具体的なファイル・コマンド）

Windows での環境セットアップ（PowerShell）:

```powershell
# 環境変数のみ設定（VS Code を起動しない場合）
. .\Start-VSCode-With-Env.ps1 -EnvOnly

# 環境変数を設定して VS Code を起動
.\Start-VSCode-With-Env.ps1
```

`Start-VSCode-With-Env.ps1` が自動設定する環境変数:

| 環境変数 | 内容 |
|---------|------|
| `PATH` | Git for Windows（MinGW）のバイナリパスを追加 |
| `INCLUDE` | Visual Studio Build Tools のインクルードパス |
| `LIB` | Visual Studio Build Tools のライブラリパス |

WSL を使った Linux ビルドの確認:

```bash
# WSL ターミナルから
gcc --version     # GCC のバージョン確認
make --version    # Make のバージョン確認
make              # ビルド実行
```

OS による出力ファイルの違い:

| 種別 | Linux | Windows |
|------|-------|---------|
| 動的ライブラリ | `libcalc.so` | `libcalc.dll` |
| 静的ライブラリ | `libcalcbase.a` | `libcalcbase.lib` |
| 実行ファイル | `add` | `add.exe` |

### 関連ドキュメント

- [VS Code（スキルガイド）](vscode.md) — VS Code の設定と拡張機能
- [GCC / MSVC ツールチェイン（スキルガイド）](../04-build-system/gcc-toolchain.md) — コンパイラのオプション
- [クロスプラットフォーム対応（スキルガイド）](../03-c-language/c-cross-platform.md) — OS 差異の吸収方法
