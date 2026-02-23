#include <libbase.h>
#include <stdio.h>

int funcman_info(funcman_object *const *fobj_array, const size_t fobj_length)
{
    int rtc = 0;
    size_t fobj_index;

    for (fobj_index = 0; fobj_index < fobj_length; fobj_index++)
    {
        funcman_object *fobj = fobj_array[fobj_index];

        if (fobj->resolved == 0)
        {
            (void)_funcman_get_func(fobj);
        }

        printf("- [%zu] %s\n", fobj_index, fobj->func_key);
        printf("    - resolved : %d\n", fobj->resolved);
        if (fobj->resolved < 0)
        {
            rtc = -1;
        }
        printf("    - lib_name : %s\n", fobj->lib_name);
        printf("    - func_name: %s\n", fobj->func_name);
        printf("    - handle   : %p\n", (void *)fobj->handle);
        printf("    - func_ptr : %p\n", fobj->func_ptr);
    }

    return rtc;
}
