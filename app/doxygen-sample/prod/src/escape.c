/**
 *  @file           escape.c
 *  @brief          エスケープのサンプル
 * 
 *  例えば、${ENV_NAME} のような文字列をドキュメントに含めたい場合、
 *  Markdown 変換時にはエスケープする必要があります。
 * 
 *  エスケープが必要な書式の例
 * 
 *  - ${ENV_NAME} (${ENV_NAME})
 * 
 *  ただし、数式内の $VAR や LaTeX コマンドの `$\command` などはエスケープ不要です。
 */
