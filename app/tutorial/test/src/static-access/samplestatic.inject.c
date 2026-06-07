// テスト対象ソース ファイルの注入用追加ソース
// このソースはテスト対象ソースの末尾に結合されます
// この static メンバーへのアクセサーによって
// テスト プログラムからテスト対象ソースの static メンバーにアクセスできます
#ifndef _IN_TEST_SRC
#include "samplestatic.c"
#endif /* _IN_TEST_SRC */

#include "samplestatic.inject.h"

void set_static_int(int set_value)
{
    static_int = set_value;
}
