/**
 *******************************************************************************
 *  @file           arena.h
 *  @brief          実行時に組み立てる記述子の記憶域をまとめて確保、解放します。
 *
 *  記述子、フィールド配列、属性配列、文字列は互いを指し合い、寿命が完全に一致します。
 *  個別に解放すると解放漏れと二重解放の余地が生まれるため、1 個のアリーナから確保し、
 *  カタログの破棄でまとめて返します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_CATALOG_ARENA_H
#define STRUCT_META_CATALOG_ARENA_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    typedef struct struct_meta_internal_arena struct_meta_internal_arena;

    /**
     *  @brief          アリーナを作ります。
     *  @return         アリーナです。確保できない場合は NULL を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    struct_meta_internal_arena *struct_meta_internal_arena_create(void);

    /**
     *  @brief          アリーナから記憶域を確保します。
     *  @param[in,out]  arena      確保元。NULL を渡してはなりません。
     *  @param[in]      size       確保するバイト数。0 を渡してはなりません。
     *  @param[in]      alignment  必要なアラインメント。2 の冪でなければなりません。
     *  @return         確保した領域です。確保できない場合は NULL を返します。
     *
     *  返した領域はゼロ初期化済みです。個別には解放できません。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  異なるアリーナに対する呼び出しは同時に実行できます。\n
     *  同一アリーナに対する操作は、呼び出し側で同期してください。
     */
    void *struct_meta_internal_arena_allocate(struct_meta_internal_arena *arena, size_t size, size_t alignment);

    /**
     *  @brief          文字列をアリーナへ複写します。
     *  @param[in,out]  arena  確保元。NULL を渡してはなりません。
     *  @param[in]      text   複写する文字列。NULL を渡せます。
     *  @return         複写した文字列です。@p text が NULL なら NULL を返します。
     *                  確保できない場合も NULL を返します。
     *
     *  @attention      @p text が NULL のときと確保に失敗したときで戻り値が同じです。
     *                  呼び出し側は @p text の NULL を先に判定してください。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  異なるアリーナに対する呼び出しは同時に実行できます。\n
     *  同一アリーナに対する操作は、呼び出し側で同期してください。
     */
    char *struct_meta_internal_arena_copy_string(struct_meta_internal_arena *arena, const char *text);

    /**
     *  @brief          アリーナと、そこから確保したすべての領域を解放します。
     *  @param[in,out]  arena  解放するアリーナ。NULL を渡せます。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  異なるアリーナに対する呼び出しは同時に実行できます。\n
     *  同一アリーナに対する操作は、呼び出し側で直列化してください。
     */
    void struct_meta_internal_arena_destroy(struct_meta_internal_arena *arena);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STRUCT_META_CATALOG_ARENA_H */
