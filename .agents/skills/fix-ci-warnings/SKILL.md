---
name: fix-ci-warnings
description: |
  CI 警告を対策するために使用するスキルです。
  ビルドは成功したものの、警告が残っている場合、
  このスキルに従って対処を行います。
when_to_use: |
  - CI 警告を修正したいとき。
---

# CI 警告修正

## チェック対象ファイル

以下のファイルをチェックし、ファイルが存在する場合、警告ありのため、内容を確認し対処する。

- https://hondarer.github.io/c-modernization-kit/artifacts/docs-warns.zip      
- https://hondarer.github.io/c-modernization-kit/artifacts/linux-ol10-warns.zip
- https://hondarer.github.io/c-modernization-kit/artifacts/linux-ol8-warns.zip 
- https://hondarer.github.io/c-modernization-kit/artifacts/windows-warns.zip  

## 注意事項

ビルドエラーの場合、アーティファクトは保存されないため、このドキュメントはあくまでもビルド成功＋警告の対処の場合の手続きとする。

修正方針は、他のファイルの類似個所をチェックし、レベル感が乖離しないようにすること。

修正方針は、修正前にユーザーに確認を行うこと。

修正後のビルドは、最低範囲のコンパイルチェックは必須。全体ビルドやテストはユーザーに確認を行うこと。

## 警告別の修正方針

### `-Wpadded` (padding struct / padding struct size)

`#pragma GCC diagnostic ignored "-Wpadded"` は原則使用しない。
変数宣言の順序変更や明示的なパディングメンバー追加によって暗黙パディングを排除すること。

**対処方法:**
- 大きいアライメント順にメンバーを並べ替えて暗黙パディングをなくす。
- 並べ替えだけで解消できない場合は、不足バイト分の明示的なパディングメンバー（例: `uint32_t padding;`）を追加する。
  - パディングメンバーの命名は `padding` を基本とする。
