#include <func_manager.h>
#include "func_manager_local.h"

/* doxygen コメントは、ヘッダに記載 */
void func_manager_init(func_object *const *caches, const size_t count, const char *configpath)
{
    _func_objects = caches;
    _func_objects_count = count;
    func_manager_load_config(configpath);
}
