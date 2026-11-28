# service-sample

`cplat/service/service.h` を利用したクロスプラットフォーム サービス (デーモン) サンプルです。

## モジュール仕様書

### Doxygen

<!-- docsfw の仕上がりパスに対する相対リンク。この Markdown からの相対パスではないことに注意 -->

生成前またはリンク先を参照できない場合は、[Doxygen の生成入口](../prod/README.md) と [doxyfw の生成手順](../../../framework/doxyfw/docs/makefile-usage.md) を参照してください。

- [service-sample](../../../doxygen/service-sample/index.html)
    - [ファイルの一覧](../../../doxygen/service-sample/files.html)

### 単一ファイル版

- [service-sample](doxybook2/README.md)
    - [ファイルの一覧](doxybook2/Files/README.md)

## 関連ドキュメント

サービス基盤のイベント仕様と OOM killer 対策は、`app/cplat` 側を正本とします。

- [サービス イベントのタイミング](../../cplat/docs/service-events.md)
- [Linux systemd OOM killer 対策アドバイザリ](../../cplat/docs/systemd-oomkiller-advisory.md)
- [cplat サービス機能仕様](../../cplat/docs/functional-spec/service.md)

\toc depth=-1 exclude-basedir=true exclude="doxybook2/*"
