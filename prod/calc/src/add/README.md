# calc/src/add/

add 関数の呼び出しコマンド。

## .so の認識

コマンド実行にあたって、lib にパスを通しておく必要があります。

```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`cd ../../lib && pwd`
```
