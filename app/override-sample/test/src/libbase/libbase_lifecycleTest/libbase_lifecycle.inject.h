#ifndef LIBBASE_LIFECYCLE_INJECT_H
#define LIBBASE_LIFECYCLE_INJECT_H

#ifdef __cplusplus
extern "C"
{
#endif
    /* ライブラリ ロード時の処理 onLoad() を呼び出す。 */
    void test_libbase_on_load(void);

    /* ロード時の診断の出力 (CPLAT_DLLMAIN_INFO_MSG) を呼び出す。 */
    void test_libbase_info_msg(const char *msg);
#ifdef __cplusplus
}
#endif

#endif /* LIBBASE_LIFECYCLE_INJECT_H */
