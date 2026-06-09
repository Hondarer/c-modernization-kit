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

ビルド エラーの場合、アーティファクトは保存されないため、このドキュメントはあくまでもビルド成功＋警告の対処の場合の手続きとする。

修正方針は、他のファイルの類似個所をチェックし、レベル感が乖離しないようにすること。

修正方針は、修正前にユーザーに確認を行うこと。

修正後のビルドは、最低範囲のコンパイル チェックは必須。全体ビルドやテストはユーザーに確認を行うこと。

## 警告別の修正方針

### -Wpadded (padding struct / padding struct size)

`#pragma GCC diagnostic ignored "-Wpadded"` は原則使用しない。  
変数宣言の順序変更や明示的なパディング メンバー追加によって暗黙パディングを排除すること。

**対処方法:**

- 可読性を損なわない範囲でメンバー配置を見直す。
- 積極的な並び替えよりも可読性を優先し、並びを保ちたい場合は不足バイト分の明示的なパディング メンバー (例: `unsigned int pad;`) を追加する。
    - パディング メンバーの命名は `pad` を基本とし、複数ある場合は `pad1`, `pad2`, ... とする。
    - 条件付きパディングが必要な場合は `#if defined(ARCH_X64)` など `platform.h` の統一マクロを使う。
