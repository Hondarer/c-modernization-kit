#include <libbase.h>
#include <stdio.h>

void funcman_info(funcman_object *const *fobj_array, const size_t fobj_length)
{
    size_t fobj_index;

    for (fobj_index = 0; fobj_index < fobj_length; fobj_index++)
    {
        funcman_object *fobj = fobj_array[fobj_index];
        printf("- %s\n", fobj->func_key);
        printf("    - lib_name : %s\n", fobj->lib_name);
        printf("    - func_name: %s\n", fobj->func_name);

        if (fobj->func_ptr == NULL)
        {
            (void)_funcman_get_func(fobj);
        }

        printf("    - handle   : %p\n", (void *)fobj->handle);
        printf("    - func_ptr : %p\n", fobj->func_ptr);
    }

    return;
}
