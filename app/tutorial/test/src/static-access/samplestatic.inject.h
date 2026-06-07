// テスト対象ソース ファイルの注入用追加ヘッダー
// このソースはテスト対象ソースの先頭に結合されます
// このヘッダーをテスト プログラムが参照することで
// テスト プログラムからテスト対象ソースの static メンバーにアクセスできます
#ifndef SAMPLESTATIC_INJECT_H
#define SAMPLESTATIC_INJECT_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    extern void set_static_int(int);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAMPLESTATIC_INJECT_H */
