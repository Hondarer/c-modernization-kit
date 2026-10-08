/**
 *******************************************************************************
 *  @file           ast.c
 *  @brief          解析対象ヘッダーの構文解析結果を保持する AST を実装します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/parse/ast.h>

#include <stdlib.h>
#include <string.h>

/* Doxygen コメントは、ヘッダーに記載 */

struct_meta_internal_parse_field *
struct_meta_internal_parse_field_create(char *name, char *type_name, int is_struct_type, long array_count, int line,
                                        char *brief, struct_meta_internal_parse_attribute *attributes)
{
    struct_meta_internal_parse_field *field = (struct_meta_internal_parse_field *)calloc(1, sizeof(*field));
    if (field == NULL)
    {
        return NULL;
    }
    field->name = name;
    field->type_name = type_name;
    field->brief = brief;
    field->attributes = attributes;
    field->is_struct_type = is_struct_type;
    field->array_count = array_count;
    field->line = line;
    field->next = NULL;
    return field;
}

struct_meta_internal_parse_field_list *
struct_meta_internal_parse_field_list_create(struct_meta_internal_parse_field *first)
{
    struct_meta_internal_parse_field_list *list = (struct_meta_internal_parse_field_list *)calloc(1, sizeof(*list));
    if (list == NULL)
    {
        return NULL;
    }
    list->head = first;
    list->tail = first;
    return list;
}

struct_meta_internal_parse_field_list *
struct_meta_internal_parse_field_list_append(struct_meta_internal_parse_field_list *list,
                                             struct_meta_internal_parse_field *field)
{
    list->tail->next = field;
    list->tail = field;
    return list;
}

struct_meta_internal_parse_struct *
struct_meta_internal_parse_struct_create(char *name, struct_meta_internal_parse_field_list *fields, char *brief,
                                         int line, struct_meta_internal_parse_attribute *attributes)
{
    struct_meta_internal_parse_struct *item = (struct_meta_internal_parse_struct *)calloc(1, sizeof(*item));
    if (item == NULL)
    {
        return NULL;
    }
    item->name = name;
    item->brief = brief;
    item->attributes = attributes;
    item->fields = fields->head;
    item->line = line;
    item->next = NULL;
    free(fields);
    return item;
}

void struct_meta_internal_parse_struct_list_append(struct_meta_internal_parse_struct_list **list,
                                                   struct_meta_internal_parse_struct *item)
{
    if (*list == NULL)
    {
        *list = (struct_meta_internal_parse_struct_list *)calloc(1, sizeof(**list));
        if (*list == NULL)
        {
            return;
        }
    }
    if ((*list)->head == NULL)
    {
        (*list)->head = item;
        (*list)->tail = item;
    }
    else
    {
        (*list)->tail->next = item;
        (*list)->tail = item;
    }
}

const struct_meta_internal_parse_struct *
struct_meta_internal_parse_struct_list_find(const struct_meta_internal_parse_struct_list *list, const char *name)
{
    if ((list == NULL) || (name == NULL))
    {
        return NULL;
    }
    for (const struct_meta_internal_parse_struct *item = list->head; item != NULL; item = item->next)
    {
        if (strcmp(item->name, name) == 0)
        {
            return item;
        }
    }
    return NULL;
}

void struct_meta_internal_parse_attribute_list_destroy(struct_meta_internal_parse_attribute *attributes)
{
    struct_meta_internal_parse_attribute *current = attributes;
    while (current != NULL)
    {
        struct_meta_internal_parse_attribute *next = current->next;
        free(current->key);
        free(current->value);
        free(current);
        current = next;
    }
}

void struct_meta_internal_parse_field_destroy(struct_meta_internal_parse_field *fields)
{
    struct_meta_internal_parse_field *current = fields;
    while (current != NULL)
    {
        struct_meta_internal_parse_field *next = current->next;
        free(current->name);
        free(current->type_name);
        free(current->brief);
        struct_meta_internal_parse_attribute_list_destroy(current->attributes);
        free(current);
        current = next;
    }
}

void struct_meta_internal_parse_field_list_destroy(struct_meta_internal_parse_field_list *list)
{
    if (list == NULL)
    {
        return;
    }
    struct_meta_internal_parse_field_destroy(list->head);
    free(list);
}

void struct_meta_internal_parse_struct_destroy(struct_meta_internal_parse_struct *items)
{
    struct_meta_internal_parse_struct *current = items;
    while (current != NULL)
    {
        struct_meta_internal_parse_struct *next = current->next;
        free(current->name);
        free(current->brief);
        struct_meta_internal_parse_attribute_list_destroy(current->attributes);
        struct_meta_internal_parse_field_destroy(current->fields);
        free(current);
        current = next;
    }
}

void struct_meta_internal_parse_struct_list_destroy(struct_meta_internal_parse_struct_list *list)
{
    if (list == NULL)
    {
        return;
    }
    struct_meta_internal_parse_struct_destroy(list->head);
    free(list);
}
