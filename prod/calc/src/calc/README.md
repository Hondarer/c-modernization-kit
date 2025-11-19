# 動的リンクを使った関数の呼び出しコマンド

## ライブラリの認識

コマンド実行にあたって、lib にパスを通しておく必要があります。

```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`cd ../../lib && pwd`
```

```powershell
$lib = (Resolve-Path ../../lib).Path
if ($env:PATH) {
  $env:PATH += ";$lib"
} else {
  $env:PATH = $lib
}
```
