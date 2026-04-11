// テスト対象ソースファイルの注入用追加ヘッダ
// このソースはテスト対象ソースの先頭に結合される
// このヘッダをテストプログラムが参照することで
// テストプログラムからテスト対象ソースの static メンバーにアクセスできる
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
