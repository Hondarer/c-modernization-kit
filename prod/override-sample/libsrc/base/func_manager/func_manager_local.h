
#ifndef FUNC_MANAGER_LOCAL_H
#define FUNC_MANAGER_LOCAL_H

#include <func_manager.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /** func_manager が管理するキャッシュポインタ配列。 */
    extern func_object *const *_func_objects;
    /** _func_objects の要素数。 */
    extern size_t _func_objects_count;

    /**
     * テキストファイルから func_key / lib_name / func_name を読み込み、
     * 全キャッシュエントリの lib_name / func_name を設定する。
     * onLoad / constructor コンテキストで func_manager_init の後に呼ぶこと。
     * ファイルフォーマット: "func_key lib_name func_name [# コメント]"
     * @param configpath[in]  設定テキストファイルのパス (例: "/tmp/libbase_ext.txt")
     * @return 設定されたエントリ数。ファイルが開けない場合は -1。
     */
    extern int func_manager_load_config(const char *configpath);

#ifdef __cplusplus
}
#endif

#endif /* FUNC_MANAGER_LOCAL_H */
