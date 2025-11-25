// テスト対象ソースファイルの注入用追加ソース
// このソースはテスト対象ソースの末尾に結合される
// この static メンバーへのアクセサーによって
// テストプログラムからテスト対象ソースの static メンバーにアクセスできる
#ifndef _IN_TEST_SRC
#include "samplestatic.c"
#endif // _IN_TEST_SRC

#include "samplestatic.inject.h"

void set_static_int(int set_value)
{
    static_int = set_value;
}
