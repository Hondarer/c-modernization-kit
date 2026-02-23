#include <func_manager.h>
#include "func_manager_local.h"

/* doxygen コメントは、ヘッダに記載 */
void func_manager_dispose(void)
{
    size_t i;

    /* DllMain / destructor コンテキストから呼ばれるため、
     * ローダーロック保持中にミューテックスを取得すると
     * デッドロックを引き起こす恐れがある。
     * このコンテキストではシングルスレッド動作が保証されるため、
     * ロックなしで解放する。 */

    for (i = 0; i < _func_objects_count; i++)
    {
        func_object *cache = _func_objects[i];

        if (cache->handle == NULL)
        {
            continue;
        }

#ifndef _WIN32
        dlclose(cache->handle);
#else
        FreeLibrary(cache->handle);
#endif

        cache->handle = NULL;
        cache->func_ptr = NULL;
    }
}
