/**
 *  catalog.c が参照するが、struct_meta_catalog_attach_static() は呼ばない入口の代用品です。
 *  このテストは共有ライブラリをリンクしないため、未使用の呼び出し先も解決する必要があります。
 */

#include <struct_meta/catalog/build.h>
#include <struct_meta/parse/parse_internal.h>

#include <cplat/base/result.h>

int struct_meta_internal_build_descriptors(const struct_meta_internal_parse_struct_list *structs,
                                           struct_meta_internal_arena *arena,
                                           const struct_meta_descriptor *const **descriptors_out, size_t *count_out,
                                           struct_meta_diagnostic *diagnostic)
{
    (void)structs;
    (void)arena;
    (void)descriptors_out;
    (void)count_out;
    (void)diagnostic;
    return CPLAT_ERR_NOT_FOUND;
}

int struct_meta_internal_parse_header_file(const char *path, struct_meta_internal_parse_struct_list **structs_out,
                                           struct_meta_diagnostic *diagnostic_out)
{
    (void)path;
    (void)structs_out;
    (void)diagnostic_out;
    return CPLAT_ERR_NOT_FOUND;
}

int struct_meta_internal_parse_header_text(const char *text, size_t length,
                                           struct_meta_internal_parse_struct_list **structs_out,
                                           struct_meta_diagnostic *diagnostic_out)
{
    (void)text;
    (void)length;
    (void)structs_out;
    (void)diagnostic_out;
    return CPLAT_ERR_NOT_FOUND;
}
