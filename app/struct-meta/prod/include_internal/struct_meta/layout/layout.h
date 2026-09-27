/**
 *******************************************************************************
 *  @file           layout.h
 *  @brief          対応する型の表と、x86_64 の構造体レイアウト計算を提供します。
 *
 *  解析対象ヘッダーからメタデータを得る経路は 2 系統あります。生成した C ソースを
 *  実行体へ組み込む事前組み込み型と、実行時にヘッダーを構文解析する事後解析型です。\n
 *  本モジュールは、その両方が使用するレイアウトの正本です。事後解析型は本モジュール
 *  の計算値をそのまま記述子へ入れ、事前組み込み型は生成コードへ @c _Static_assert を
 *  出力して、コンパイラが決めた @c offsetof / @c sizeof と本モジュールの計算値が
 *  一致することを毎ビルド検査します。\n
 *  see: app/struct-meta/docs/architecture.md
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_LAYOUT_LAYOUT_H
#define STRUCT_META_LAYOUT_LAYOUT_H

#include <struct_meta/meta/meta.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 *  @brief          対応する型 1 個分の、種別と配置に必要な情報です。
 */
typedef struct struct_meta_internal_layout_type
{
    const char *spelling;           /**< 解析対象ヘッダーに書かれる型スペリングです。 */
    const char *canonical_spelling; /**< 診断へ出す、正規化した型スペリングです。 */
    struct_meta_field_kind kind;    /**< 対応するフィールド種別です。 */
    unsigned int pad;               /**< 明示的アラインメントです。0 を指定します。 */
    size_t size;                    /**< 要素 1 個のバイト数です。 */
    size_t alignment;               /**< 要素 1 個のアラインメントです。 */
} struct_meta_internal_layout_type;

/**
 *  @brief          型スペリングから、対応する型の情報を求めます。
 *  @param[in]      spelling  型スペリング。NULL を渡せます。
 *  @return         対応する情報です。NULL または対応しない型名では NULL を返します。
 *
 *  `char` はスカラーでは符号付き整数、配列では既定で文字列として扱います。種別の
 *  最終決定は呼び出し側が配列かどうかと属性を見て行い、本表は宣言型だけを表します。\n
 *  `long` と `unsigned long` は LP64 と LLP64 で幅が異なり、生成物のプラットフォーム間
 *  互換性を壊すため、この表に含めません。
 *
 *  @par            スレッド セーフ
 *  本関数はスレッド セーフです。
 */
const struct_meta_internal_layout_type *struct_meta_internal_layout_find_type(const char *spelling);

/**
 *  @brief          構造体レイアウトを求める途中経過です。
 */
typedef struct struct_meta_internal_layout_builder
{
    size_t offset;    /**< 次のメンバーの候補オフセットです。 */
    size_t alignment; /**< ここまでのメンバーの最大アラインメントです。 */
} struct_meta_internal_layout_builder;

/**
 *  @brief          構造体レイアウトの計算を開始します。
 *  @param[out]     builder  初期化する途中経過。NULL を渡してはなりません。
 *
 *  @par            スレッド セーフ
 *  本関数はスレッド セーフです。
 */
void struct_meta_internal_layout_begin(struct_meta_internal_layout_builder *builder);

/**
 *  @brief          メンバーを 1 個追加し、そのオフセットを求めます。
 *  @param[in,out]  builder        途中経過。NULL を渡してはなりません。
 *  @param[in]      element_size   要素 1 個のバイト数。0 を渡してはなりません。
 *  @param[in]      element_count  要素数。スカラーでは 1 です。0 を渡してはなりません。
 *  @param[in]      alignment      メンバーのアラインメント。2 の冪でなければなりません。
 *  @param[out]     offset_out     求めたオフセットの格納先。NULL を渡してはなりません。
 *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、または
 *                  @c CPLAT_ERR_OUT_OF_RANGE を返します。
 *
 *  現在のオフセットを @p alignment へ切り上げた位置をメンバーの先頭とし、
 *  そこへ @p element_size × @p element_count を加えた位置を次の候補とします。\n
 *  配列のアラインメントは要素のアラインメントと同じです。
 *
 *  @par            スレッド セーフ
 *  本関数はスレッド セーフです。
 */
int struct_meta_internal_layout_add(struct_meta_internal_layout_builder *builder, size_t element_size,
                                    size_t element_count, size_t alignment, size_t *offset_out);

/**
 *  @brief          構造体レイアウトの計算を終え、大きさとアラインメントを求めます。
 *  @param[in]      builder        途中経過。NULL を渡してはなりません。
 *  @param[out]     size_out       構造体全体のバイト数の格納先。NULL を渡してはなりません。
 *  @param[out]     alignment_out  構造体のアラインメントの格納先。NULL を渡してはなりません。
 *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、または
 *                  @c CPLAT_ERR_OUT_OF_RANGE を返します。
 *
 *  構造体のアラインメントは最大メンバー アラインメント、大きさは末尾をそのアラインメント
 *  へ切り上げた値です。メンバーを 1 個も追加していない場合は
 *  @c CPLAT_ERR_INVALID_ARGUMENT を返します。C ではメンバーの無い構造体を宣言できません。
 *
 *  @par            スレッド セーフ
 *  本関数はスレッド セーフです。
 */
int struct_meta_internal_layout_end(const struct_meta_internal_layout_builder *builder, size_t *size_out,
                                    size_t *alignment_out);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STRUCT_META_LAYOUT_LAYOUT_H */
