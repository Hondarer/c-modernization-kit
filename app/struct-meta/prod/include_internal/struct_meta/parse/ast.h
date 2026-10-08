/**
 *******************************************************************************
 *  @file           ast.h
 *  @brief          解析対象ヘッダーの構文解析結果を保持する AST を定義します。
 *
 *  構文解析器はフル C パーサーではなく、`typedef struct { ... } Name;` の形に
 *  限定した宣言だけを解析対象とします。詳細は
 *  `app/struct-meta/docs/architecture.md` を参照してください。\n
 *  AST は宣言をそのまま写したものであり、レイアウトを含みません。オフセットと
 *  大きさは `catalog` が `layout` を使って求めます。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_PARSE_AST_H
#define STRUCT_META_PARSE_AST_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          型指定 1 個分の解析結果です (bison の型付き値として使用)。
     */
    typedef struct struct_meta_internal_parse_typespec
    {
        char *name;    /**< 型のスペリングです (@c struct_meta_internal_layout_find_type が扱う型名、
                     *   または他の `typedef struct` の名前 (ネスト メンバー))。 */
        int is_struct; /**< 1 なら `name` は同一ヘッダー内の構造体名 (ネスト メンバー) です。 */
        int pad;       /**< 明示的アラインメントです。0 を指定します。 */
    } struct_meta_internal_parse_typespec;

    /**
     *  @brief          `@struct_meta{key}` または `@struct_meta{key=value}` 1 個分の解析結果です。
     */
    typedef struct struct_meta_internal_parse_attribute
    {
        char *key;   /**< 属性名です。 */
        char *value; /**< 属性値です。値を持たない場合は NULL です。 */
        struct struct_meta_internal_parse_attribute *next;
    } struct_meta_internal_parse_attribute;

    /**
     *  @brief          構造体 1 フィールド分の解析結果です。
     */
    typedef struct struct_meta_internal_parse_field
    {
        char *name;      /**< フィールド名です。 */
        char *type_name; /**< 型のスペリングです (プリミティブの綴り、または構造体名)。 */
        char *brief;     /**< Doxygen から取り出した短い説明です。無いときは NULL です。 */
        struct struct_meta_internal_parse_attribute *attributes; /**< Doxygen から取り出した汎用属性です。 */
        long array_count;                                        /**< `[N]` の N です。スカラー フィールドは 0 です。 */
        int line;                                                /**< ソース上の行番号です (診断メッセージ用)。 */
        int is_struct_type; /**< 1 なら `type_name` は同一ヘッダー内の構造体名 (ネスト メンバー) です。 */
        struct struct_meta_internal_parse_field *next;
    } struct_meta_internal_parse_field;

    /**
     *  @brief          `typedef struct { ... } Name;` 1 個分の解析結果です。
     */
    typedef struct struct_meta_internal_parse_struct
    {
        char *name;  /**< 構造体名 (typedef 名) です。 */
        char *brief; /**< Doxygen から取り出した短い説明です。無いときは NULL です。 */
        struct struct_meta_internal_parse_attribute *attributes; /**< Doxygen から取り出した汎用属性です。 */
        struct_meta_internal_parse_field *fields;                /**< フィールドの連結リストです。 */
        int line;                                                /**< ソース上の行番号です (診断メッセージ用)。 */
        int pad;                                                 /**< 明示的アラインメントです。0 を指定します。 */
        struct struct_meta_internal_parse_struct *next;
    } struct_meta_internal_parse_struct;

    /**
     *  @brief          フィールド リストを構築するための一時ハンドルです (末尾ポインターを保持)。
     */
    typedef struct struct_meta_internal_parse_field_list
    {
        struct_meta_internal_parse_field *head;
        struct_meta_internal_parse_field *tail;
    } struct_meta_internal_parse_field_list;

    /**
     *  @brief          構造体リストを構築するための一時ハンドルです (末尾ポインターを保持)。
     */
    typedef struct struct_meta_internal_parse_struct_list
    {
        struct_meta_internal_parse_struct *head;
        struct_meta_internal_parse_struct *tail;
    } struct_meta_internal_parse_struct_list;

    struct_meta_internal_parse_field *
    struct_meta_internal_parse_field_create(char *name, char *type_name, int is_struct_type, long array_count, int line,
                                            char *brief, struct_meta_internal_parse_attribute *attributes);
    struct_meta_internal_parse_field_list *
    struct_meta_internal_parse_field_list_create(struct_meta_internal_parse_field *first);
    struct_meta_internal_parse_field_list *
    struct_meta_internal_parse_field_list_append(struct_meta_internal_parse_field_list *list,
                                                 struct_meta_internal_parse_field *field);

    struct_meta_internal_parse_struct *
    struct_meta_internal_parse_struct_create(char *name, struct_meta_internal_parse_field_list *fields, char *brief,
                                             int line, struct_meta_internal_parse_attribute *attributes);
    void struct_meta_internal_parse_struct_list_append(struct_meta_internal_parse_struct_list **list,
                                                       struct_meta_internal_parse_struct *item);
    const struct_meta_internal_parse_struct *
    struct_meta_internal_parse_struct_list_find(const struct_meta_internal_parse_struct_list *list, const char *name);

    /**
     *  @brief          構造体リストと、そこから辿れる要素をすべて解放します。
     *  @param[in,out]  list  解放するリスト。NULL を渡せます。
     *
     *  構文解析が途中で失敗した場合も、ここまでに作った要素を解放できます。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  異なるリストに対する呼び出しは同時に実行できます。\n
     *  同一リストに対する操作は、呼び出し側で直列化してください。
     */
    void struct_meta_internal_parse_struct_list_destroy(struct_meta_internal_parse_struct_list *list);

    /**
     *  @brief          属性の連結リストを解放します。
     *  @param[in,out]  attributes  解放するリスト。NULL を渡せます。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  異なるリストに対する呼び出しは同時に実行できます。\n
     *  同一リストに対する操作は、呼び出し側で直列化してください。
     */
    void struct_meta_internal_parse_attribute_list_destroy(struct_meta_internal_parse_attribute *attributes);

    /**
     *  @brief          フィールドの連結リストを解放します。
     *  @param[in,out]  fields  解放する先頭要素。NULL を渡せます。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  異なるフィールドに対する呼び出しは同時に実行できます。\n
     *  同一フィールドに対する操作は、呼び出し側で直列化してください。
     */
    void struct_meta_internal_parse_field_destroy(struct_meta_internal_parse_field *fields);

    /**
     *  @brief          フィールド リストのハンドルと、その要素をすべて解放します。
     *  @param[in,out]  list  解放するハンドル。NULL を渡せます。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  異なるリストに対する呼び出しは同時に実行できます。\n
     *  同一リストに対する操作は、呼び出し側で直列化してください。
     */
    void struct_meta_internal_parse_field_list_destroy(struct_meta_internal_parse_field_list *list);

    /**
     *  @brief          構造体の連結リストを解放します。
     *  @param[in,out]  items  解放する先頭要素。NULL を渡せます。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  異なる構造体に対する呼び出しは同時に実行できます。\n
     *  同一構造体に対する操作は、呼び出し側で直列化してください。
     */
    void struct_meta_internal_parse_struct_destroy(struct_meta_internal_parse_struct *items);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STRUCT_META_PARSE_AST_H */
