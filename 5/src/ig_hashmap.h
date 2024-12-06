#ifndef IGHMAP_HEADER_H
#define IGHMAP_HEADER_H

#include <stdlib.h>

#include "ig_enums.h"
#include "log.h"

#define IGHMAP_IMPL(word) IGHMAP_COMB1(IGHMAP_PREFIX, word)
#define IGHMAP_COMB1(pre, word) IGHMAP_COMB2(pre, word)
#define IGHMAP_COMB2(pre, word) pre##word

#define IS_PRIMITIVE_INT 1
#define IS_PRIMITIVE_LONG 1
#define IS_PRIMITIVE_SHORT 1
#define IS_PRIMITIVE_CHAR 1
#define IS_PRIMITIVE_UNSIGNED_INT 1
#define IS_PRIMITIVE_UNSIGNED_LONG 1
#define IS_PRIMITIVE_UNSIGNED_SHORT 1
#define IS_PRIMITIVE_UNSIGNED_CHAR 1
#define IS_PRIMITIVE_FLOAT 1
#define IS_PRIMITIVE_DOUBLE 1

#define IS_PRIMITIVE_CUSTOM 0

#define IS_PRIMITIVE_TYPE(type) IS_PRIMITIVE_##type

#endif

#ifndef IGHMAP_KEY_T
#error "IGHMAP_KEY_T not defined!"
#endif

#ifndef IGHMAP_VALUE_T
#error "IGHMAP_VALUE_T not defined!"
#endif

#ifndef IGHMAP_NAME
#define IGHMAP_NAME IGHMAP_COMB1(IGHMAP_COMB1(IGHMAP_COMB1(HMAP, _), IGHMAP_VALUE_T), IGHMAP_KEY_T)
#endif

#define IGHMAP_PAIRNAME IGHMAP_COMB1(IGHMAP_COMB1(HMAP_PAIRS, _), IGHMAP_NAME)

#ifndef IGHMAP_LINKAGE
#define IGHMAP_LINKAGE static inline
#endif

#define IGHMAP_KEY_IS_PRIMITIVE IS_PRIMITIVE_TYPE(IGHMAP_KEY_T)

typedef struct IGHMAP_PAIRNAME IGHMAP_PAIRNAME;
struct IGHMAP_PAIRNAME
{
    IGHMAP_KEY_T key;
    IGHMAP_VALUE_T value;
    struct IGHMAP_PAIRNAME *next;
};

typedef struct IGHMAP_NAME
{
    IGHMAP_PAIRNAME **buckets;
    size_t capacity;
    size_t size;
    float load_factor;
    size_t (*hash_func)(IGHMAP_KEY_T key, size_t capacity);
    IGBool (*comp_func)(IGHMAP_KEY_T *key, IGHMAP_KEY_T *other);

} IGHMAP_NAME;
#define IGHMAP_init IGHMAP_IMPL(init)
#define IGHMAP_insert IGHMAP_IMPL(insert)
#define IGHMAP_get IGHMAP_IMPL(get)
#define IGHMAP_resize IGHMAP_IMPL(resize)
#define IGHMAP_destroy IGHMAP_IMPL(destroy)

size_t default_hash(IGHMAP_KEY_T *key, size_t capacity)
{
#if IGHMAP_KEY_IS_PRIMITIVE
    return *key % capacity;
#else
    log_message(LOG_LEVEL_ERROR, "default_hash not supported for non-primitive types. Please provide your own!");
    return -1;
#endif
}

IGBool default_comp(IGHMAP_KEY_T *key, IGHMAP_KEY_T *other)
{
#if IGHMAP_KEY_IS_PRIMITIVE
    return *key == *other ? IGTrue : IGFalse;
#else
    log_message(LOG_LEVEL_ERROR, "default_comp not supported for non-primitive types. Please provide your own!");
    return IGFalse;
#endif
}

#ifdef IGHMAP_DECLS_ONLY

IGHMAP_LINKAGE
IGHMAP_NAME *IGHMAP_init();
IGHMAP_LINKAGE
IGHMAP_VALUE_T IGHMAP_get(IGHMAP_NAME *hmap, IGHMAP_KEY_T key);
IGHMAP_LINKAGE
void IGHMAP_insert(IGHMAP_NAME *hmap, IGHMAP_KEY_T key, IGHMAP_VALUE_T value);
IGHMAP_LINKAGE
void IGHMAP_resize(IGHMAP_NAME *hmap);
IGHMAP_LINKAGE
void IGHMAP_destroy(IGHMAP_NAME *hmap);

#else

IGHMAP_LINKAGE
IGHMAP_NAME *IGHMAP_init(size_t (*hash_func)(IGHMAP_KEY_T *, size_t), IGBool (*comp_func)(IGHMAP_KEY_T *, IGHMAP_KEY_T *))
{
    IGHMAP_NAME *hmap = malloc(sizeof(IGHMAP_NAME));
    if (!hmap)
        return NULL;

    hmap->capacity = 16; // Default initial capacity
    hmap->size = 0;
    hmap->load_factor = 0.75f;
    hmap->buckets = calloc(hmap->capacity, sizeof(IGHMAP_PAIRNAME *));
    hmap->hash_func = hash_func ? hash_func : default_hash;
    hmap->comp_func = comp_func ? comp_func : default_comp;
    if (!hmap->buckets)
    {
        free(hmap);
        return NULL;
    }
    return hmap;
}

IGHMAP_LINKAGE
void IGHMAP_resize(IGHMAP_NAME *hmap)
{
    size_t new_capacity = hmap->capacity * 2;
    IGHMAP_PAIRNAME **new_buckets = calloc(new_capacity, sizeof(IGHMAP_PAIRNAME *));
    if (!new_buckets)
        return;

    for (size_t i = 0; i < hmap->capacity; i++)
    {
        IGHMAP_PAIRNAME *node = hmap->buckets[i];
        while (node)
        {
            IGHMAP_PAIRNAME *next = node->next;
            size_t index = hmap->hash_func(node->key, new_capacity);

            node->next = new_buckets[index];
            new_buckets[index] = node;

            node = next;
        }
    }

    free(hmap->buckets);
    hmap->buckets = new_buckets;
    hmap->capacity = new_capacity;
}

IGHMAP_LINKAGE
void IGHMAP_insert(IGHMAP_NAME *hmap, IGHMAP_KEY_T key, IGHMAP_VALUE_T value)
{
    if (!hmap || !hmap->buckets)
        return;

    if ((float)(hmap->size + 1) / hmap->capacity > hmap->load_factor)
        IGHMAP_resize(hmap);

    size_t index = hmap->hash_func(key, hmap->capacity);

    IGHMAP_PAIRNAME *node = hmap->buckets[index];
    while (node)
    {
        if (hmap->comp_func(&node->key, &key))
        {
            node->value = value; // Update existing value
            return;
        }
        node = node->next;
    }

    IGHMAP_PAIRNAME *new_node = malloc(sizeof(IGHMAP_PAIRNAME));
    if (!new_node)
        return;

    new_node->key = key;
    new_node->value = value;
    new_node->next = hmap->buckets[index];
    hmap->buckets[index] = new_node;
    hmap->size++;
}

IGHMAP_LINKAGE
IGHMAP_VALUE_T IGHMAP_get(IGHMAP_NAME *hmap, IGHMAP_KEY_T key)
{
    size_t index = hmap->hash_func(key, hmap->capacity);
    IGHMAP_PAIRNAME *node = hmap->buckets[index];
    while (node)
    {
        if (hmap->comp_func(&node->key, &key))
            return node->value;
        node = node->next;
    }
}

IGHMAP_LINKAGE
void IGHMAP_destroy(IGHMAP_NAME *hmap)
{
    if (!hmap)
        return;

    // Free all buckets
    for (size_t i = 0; i < hmap->capacity; ++i)
    {
        IGHMAP_PAIRNAME *node = hmap->buckets[i];
        while (node)
        {
            IGHMAP_PAIRNAME *temp = node;
            node = node->next;
            free(temp); // Free each linked list node
        }
    }

    free(hmap->buckets); // Free the bucket array
    free(hmap);          // Free the hash map structure
}

#endif

// Cleanup

#undef IGHMAP_T
#undef IGHMAP_PREFIX
#undef IGHMAP_NAME
#undef IGHMAP_LINKAGE
#ifdef IGHMAP_DECLS_ONLY
#undef IGHMAP_DECLS_ONLY
#endif