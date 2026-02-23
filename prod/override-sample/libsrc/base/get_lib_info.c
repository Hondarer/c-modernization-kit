/**
 * @file get_lib_info.c
 * @brief 共有ライブラリ自身(.so/.dll)の絶対パスと basename(パスなし・拡張子なし)を取得するクロスプラットフォーム API
 *
 * Linux(gcc) では dladdr() で共有オブジェクトを特定し、realpath() で可能な限り正規化(絶対化・symlink解決)します。
 * Windows(MSVC) では GetModuleHandleEx() で DLL の HMODULE を得て、GetModuleFileNameW() でパスを取得します。
 * 取得したパスは UTF-8 で返却します。
 *
 * @note
 * - 「絶対パス」は OS/ローダの情報とファイルシステム状態に依存します。
 *   Linux でロード後にファイルが移動/削除される等により realpath()
 * が失敗する場合、可能な範囲で絶対化した文字列を返します。
 * - Windows は基本的にフルパスが得られますが、古い環境では MAX_PATH 制約が残る場合があります。
 */

#include <libbase.h>
#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <stddef.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <string.h>
    #include <windows.h>
#else
    #include <dlfcn.h>
    #include <limits.h>
    #include <stddef.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
#endif

/**
 * @enum mylib_status_t
 * @brief API の戻り値(ステータス)
 */
typedef enum mylib_status_t
{
    /** 成功 */
    MYLIB_OK = 0,
    /** 引数不正(NULL、サイズ0など) */
    MYLIB_EINVAL = 1,
    /** バッファ不足(出力が収まらない) */
    MYLIB_ENOBUFS = 2,
    /** その他の失敗(取得不能、OS API 失敗など) */
    MYLIB_EFAIL = 3
} mylib_status_t;

/**
 * @brief 文字列を安全にコピーします(UTF-8 想定だが単なる byte 列として扱う)。
 *
 * @param[out] dst     出力バッファ
 * @param[in]  dst_sz  出力バッファサイズ[byte]
 * @param[in]  src     入力文字列(NUL終端)
 * @return mylib_status_t
 */
static mylib_status_t mylib__copy_str(char *dst, size_t dst_sz, const char *src)
{
    size_t n;

    if (!dst || dst_sz == 0 || !src)
        return MYLIB_EINVAL;
    n = strlen(src);
    if (n + 1 > dst_sz)
    {
        if (dst_sz > 0)
            dst[0] = '\0';
        return MYLIB_ENOBUFS;
    }
    memcpy(dst, src, n + 1);
    return MYLIB_OK;
}

/**
 * @brief パス文字列からファイル名部分(最後の区切り文字以降)を返します。
 *
 * @param[in] path パス
 * @return const char* ファイル名部分へのポインタ(元の文字列内)
 */
static const char *mylib__filename_part(const char *path)
{
    const char *p;
    const char *last = path;

    if (!path)
        return "";
    for (p = path; *p; ++p)
    {
        if (*p == '/' || *p == '\\')
            last = p + 1;
    }
    return last;
}

/**
 * @brief 拡張子を取り除きます(その場で書き換え)。
 *
 * @details
 * - Linux: ".so." が含まれる場合はそこから先を削除(例: libx.so.1.2.3 -> libx)
 * - Linux: 末尾が ".so" の場合は削除(例: libx.so -> libx)
 * - その他: 最後の '.' 以降を削除(一般的な拡張子扱い)
 *
 * @param[in,out] s 対象文字列(NUL終端)
 */
static void mylib__strip_extension_inplace(char *s)
{
    char *dot;

    if (!s)
        return;

#if !defined(_WIN32)
    {
        char *so_ver = strstr(s, ".so.");
        if (so_ver)
        {
            *so_ver = '\0';
            return;
        }

        {
            size_t len = strlen(s);
            if (len >= 3 && strcmp(s + (len - 3), ".so") == 0)
            {
                s[len - 3] = '\0';
                return;
            }
        }

        {
            size_t len = strlen(s);
            if (len >= 6 && strcmp(s + (len - 6), ".dylib") == 0)
            {
                s[len - 6] = '\0';
                return;
            }
        }
    }
#endif

    dot = strrchr(s, '.');
    if (dot && dot != s)
    {
        *dot = '\0';
    }
}

#if defined(_WIN32)

/**
 * @brief Windows ワイド文字列(UTF-16)を UTF-8 に変換します。
 *
 * @param[in]  w          入力(UTF-16、NUL終端)
 * @param[out] out_u8     出力(UTF-8、NUL終端)
 * @param[in]  out_u8_sz  出力バッファサイズ[byte]
 * @return mylib_status_t
 */
static mylib_status_t mylib__narrow_utf8(const wchar_t *w, char *out_u8, size_t out_u8_sz)
{
    int need;
    if (!w || !out_u8 || out_u8_sz == 0)
        return MYLIB_EINVAL;

    need = WideCharToMultiByte(CP_UTF8, 0, w, -1, NULL, 0, NULL, NULL);
    if (need <= 0)
        return MYLIB_EFAIL;
    if ((size_t)need > out_u8_sz)
        return MYLIB_ENOBUFS;

    if (WideCharToMultiByte(CP_UTF8, 0, w, -1, out_u8, (int)out_u8_sz, NULL, NULL) <= 0)
        return MYLIB_EFAIL;
    return MYLIB_OK;
}

/**
 * @brief DLL 自身の絶対パス(ワイド文字列)を取得します。
 *
 * @param[out] out_w      出力(UTF-16、NUL終端)
 * @param[in]  out_w_cap  出力バッファサイズ[wchar_t 個数]
 * @param[in]  func_addr  所属モジュールを特定するための関数アドレス
 * @return mylib_status_t
 */
static mylib_status_t mylib__get_self_path_w(wchar_t *out_w, size_t out_w_cap, const void *func_addr)
{
    HMODULE hm = NULL;
    wchar_t buf[MAX_PATH];
    DWORD n;

    if (!out_w || out_w_cap == 0 || !func_addr)
        return MYLIB_EINVAL;

    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)func_addr, &hm))
    {
        return MYLIB_EFAIL;
    }

    n = GetModuleFileNameW(hm, buf, (DWORD)(sizeof(buf) / sizeof(buf[0])));
    if (n == 0 || n >= (DWORD)(sizeof(buf) / sizeof(buf[0])))
    {
        return MYLIB_EFAIL;
    }
    buf[n] = L'\0';

    /**
     * GetModuleFileNameW は通常フルパスを返しますが、念のため GetFullPathNameW で正規化します。
     * (失敗した場合は buf をそのまま返す)
     */
    {
        wchar_t full[MAX_PATH];
        DWORD m = GetFullPathNameW(buf, (DWORD)(sizeof(full) / sizeof(full[0])), full, NULL);
        if (m == 0 || m >= (DWORD)(sizeof(full) / sizeof(full[0])))
        {
            if (wcslen(buf) + 1 > out_w_cap)
                return MYLIB_ENOBUFS;
            wcscpy_s(out_w, out_w_cap, buf);
            return MYLIB_OK;
        }
        if (wcslen(full) + 1 > out_w_cap)
            return MYLIB_ENOBUFS;
        wcscpy_s(out_w, out_w_cap, full);
        return MYLIB_OK;
    }
}

#else /* Linux/Unix */

/**
 * @brief .so 自身の絶対パスを取得します(Linux/Unix)。
 *
 * dladdr() に指定された関数アドレスを渡して所属共有オブジェクトを取得し、
 * realpath() で可能な限り絶対化・正規化します。
 *
 * @param[out] out_path     出力(UTF-8、NUL終端)
 * @param[in]  out_path_sz  出力バッファサイズ[byte]
 * @param[in]  func_addr    所属モジュールを特定するための関数アドレス
 * @return mylib_status_t
 */
static mylib_status_t mylib__get_self_path_posix(char *out_path, size_t out_path_sz, const void *func_addr)
{
    Dl_info info;
    const char *p;
    char resolved[PATH_MAX];

    if (!out_path || out_path_sz == 0 || !func_addr)
        return MYLIB_EINVAL;

    memset(&info, 0, sizeof(info));
    if (dladdr(func_addr, &info) == 0)
    {
        return MYLIB_EFAIL;
    }

    p = info.dli_fname ? info.dli_fname : "";
    if (p[0] == '\0')
        return MYLIB_EFAIL;

    /**
     * realpath() は symlink 解決と絶対化を行います。
     * ロード後にファイルが移動/削除される等で失敗する場合があるため、その場合はフォールバックします。
     */
    if (realpath(p, resolved) != NULL)
    {
        return mylib__copy_str(out_path, out_path_sz, resolved);
    }

    /* すでに絶対パスならそのまま返す(正規化はできない) */
    if (p[0] == '/')
    {
        return mylib__copy_str(out_path, out_path_sz, p);
    }

    /* 相対パスなら CWD と結合して絶対化を試みる */
    {
        char cwd[PATH_MAX];
        char joined[PATH_MAX];

        if (getcwd(cwd, sizeof(cwd)) == NULL)
            return MYLIB_EFAIL;
        if (snprintf(joined, sizeof(joined), "%s/%s", cwd, p) <= 0)
            return MYLIB_EFAIL;

        if (realpath(joined, resolved) != NULL)
        {
            return mylib__copy_str(out_path, out_path_sz, resolved);
        }

        /* 最後の手段: 結合した絶対パス文字列(正規化なし) */
        return mylib__copy_str(out_path, out_path_sz, joined);
    }
}

#endif

int get_lib_path(char *out_path, const size_t out_path_sz, const void *func_addr)
{
#if defined(_WIN32)
    wchar_t wpath[MAX_PATH];
    mylib_status_t st = mylib__get_self_path_w(wpath, (size_t)(sizeof(wpath) / sizeof(wpath[0])), func_addr);
    if (st != MYLIB_OK)
    {
        if (out_path && out_path_sz)
            out_path[0] = '\0';
        return st;
    }
    return (int)mylib__narrow_utf8(wpath, out_path, out_path_sz);
#else
    return (int)mylib__get_self_path_posix(out_path, out_path_sz, func_addr);
#endif
}

int get_lib_basename(char *out_basename, const size_t out_basename_sz, const void *func_addr)
{
    mylib_status_t st;
    char path_buf[4096];
    const char *fname;
    char tmp[1024];

    if (!out_basename || out_basename_sz == 0)
        return MYLIB_EINVAL;

    st = get_lib_path(path_buf, sizeof(path_buf), func_addr);
    if (st != MYLIB_OK)
    {
        out_basename[0] = '\0';
        return st;
    }

    fname = mylib__filename_part(path_buf);
    if (!fname || fname[0] == '\0')
    {
        out_basename[0] = '\0';
        return MYLIB_EFAIL;
    }

    /* ファイル名のみをコピーして拡張子を落とす */
    st = mylib__copy_str(tmp, sizeof(tmp), fname);
    if (st != MYLIB_OK)
    {
        out_basename[0] = '\0';
        return st;
    }

    mylib__strip_extension_inplace(tmp);

    return (int)mylib__copy_str(out_basename, out_basename_sz, tmp);
}
