// dynamic_array.h
// Include this header multiple times to implement a
// simplistic dynamic array.  Before inclusion define at
// least IGDARRAY_T to the type the dynamic array can hold.
// See IGDARRAY_NAME, IGDARRAY_PREFIX and IGDARRAY_LINKAGE for
// other customization points.
//
// If you define IGDARRAY_DECLS_ONLY, only the declarations
// of the type and its function will be declared.
//

#ifndef IGDARRAY_HEADER_H
#define IGDARRAY_HEADER_H

#include <stdlib.h>

#include "ig_enums.h"
#include "log.h"

#define IGDARRAY_IMPL(word) IGDARRAY_COMB1(IGDARRAY_PREFIX, word)
#define IGDARRAY_COMB1(pre, word) IGDARRAY_COMB2(pre, word)
#define IGDARRAY_COMB2(pre, word) pre##word

#endif

// Not guarded now since it will be included multiple times

#ifndef IGDARRAY_T
#error "IGDARRAY_T must be specified
#endif

#ifndef IGDARRAY_NAME
#define IGDARRAY_NAME IGDARRAY_COMB1(IGDARRAY_COMB1(darray, _), IGDARRAY_T)
#endif

#ifndef IGDARRAY_LINKAGE
#define IGDARRAY_LINKAGE static inline
#endif

typedef struct IGDARRAY_NAME IGDARRAY_NAME;
struct IGDARRAY_NAME
{
    IGDARRAY_T *data;
    size_t size;
    size_t capacity;
};

#define IGDARRAY_init IGDARRAY_IMPL(init)
#define IGDARRAY_push IGDARRAY_IMPL(push)
#define IGDARRAY_insert IGDARRAY_IMPL(insert)
#define IGDARRAY_copy IGDARRAY_IMPL(copy)
#define IGDARRAY_erase IGDARRAY_IMPL(erase)
#define IGDARRAY_destroy IGDARRAY_IMPL(destroy)

#ifdef IGDARRAY_DECLS_ONLY

IGDARRAY_LINKAGE
IGDARRAY_NAME *IGDARRAY_init();
IGDARRAY_LINKAGE
void IGDARRAY_push(IGDARRAY_NAME *array, IGDARRAY_T item);
IGDARRAY_LINKAGE
IGDARRAY_NAME IGDARRAY_copy(IGDARRAY_NAME *array);
IGDARRAY_LINKAGE
void IGDARRAY_insert(IGDARRAY_NAME *array, IGDARRAY_T item, size_t index, IGBool should_push_objects);
IGDARRAY_LINKAGE
void IGDARRAY_erase(IGDARRAY_NAME *array, size_t index);
IGDARRAY_LINKAGE
void IGDARRAY_destroy(IGDARRAY_NAME *array);

#else

IGDARRAY_LINKAGE
IGDARRAY_NAME *IGDARRAY_init()
{
    IGDARRAY_NAME *array = malloc(sizeof(IGDARRAY_NAME));
    array->capacity = 0;
    array->size = 0;
    array->data = NULL;
    return array;
}

IGDARRAY_LINKAGE
void IGDARRAY_insert(IGDARRAY_NAME *array, IGDARRAY_T item, size_t index, IGBool should_push_objects)
{

    if (index > array->size)
    {
        log_message(LOG_LEVEL_ERROR, "Index %zu out of bounds for IGDARRAY of size %zu.", index, array->size);
        exit(EXIT_FAILURE);
    }
    if (array->size >= array->capacity)
    {
        size_t old_cap = array->capacity;
        size_t new_cap = old_cap ? old_cap + 10 : 10;
        size_t new_size = new_cap * sizeof(IGDARRAY_T);
        array->data = realloc(array->data, new_size);
        if (!array->data)
        {
            log_message(LOG_LEVEL_ERROR, "Error reallocating memory for IGDARRAY.");
            exit(EXIT_FAILURE);
        }
        array->capacity = new_cap;
    }

    if (should_push_objects == IGTrue)
    {
        if (index < array->size)
        {
            memmove(&array->data[index + 1], &array->data[index], (array->size - index) * sizeof(IGDARRAY_T));
        }
        array->size++;
    }
    array->data[index] = item;
    if (should_push_objects == IGFalse && index == array->size)
    {
        array->size++;
    }
}

IGDARRAY_LINKAGE
IGDARRAY_NAME IGDARRAY_copy(IGDARRAY_NAME *array)
{
    IGDARRAY_NAME new_array;
    new_array.size = array->size;
    new_array.capacity = array->capacity;
    if (new_array.capacity > 0)
    {
        new_array.data = malloc(new_array.capacity * sizeof(IGDARRAY_T));
        if (!new_array.data)
        {
            log_message(LOG_LEVEL_ERROR, "Error allocating memory for IGDARRAY copy.");
            exit(EXIT_FAILURE);
        }
        memcpy(new_array.data, array->data, new_array.size * sizeof(IGDARRAY_T));
    }
    else
    {
        new_array.data = NULL;
    }

    return new_array;
}

IGDARRAY_LINKAGE
void IGDARRAY_erase(IGDARRAY_NAME *array, size_t index)
{
    if (index >= array->size)
    {
        log_message(LOG_LEVEL_ERROR, "Index %zu out of bounds for IGDARRAY of size %zu.", index, array->size);
        exit(EXIT_FAILURE);
    }
    if (index < array->size - 1)
    {
        memmove(&array->data[index], &array->data[index + 1], (array->size - index - 1) * sizeof(IGDARRAY_T));
    }
    array->size--;
    array->data[array->size] = (IGDARRAY_T){0};
}

IGDARRAY_LINKAGE
void IGDARRAY_push(IGDARRAY_NAME *array, IGDARRAY_T item)
{
    IGDARRAY_insert(array, item, array->size, IGTrue);
}

IGDARRAY_LINKAGE
void IGDARRAY_destroy(IGDARRAY_NAME *array)
{
    free(array->data);
}
#endif

// Cleanup
// These need to be undef'ed so they can be redefined the
// next time you need to instantiate this template.
#undef IGDARRAY_T
#undef IGDARRAY_PREFIX
#undef IGDARRAY_NAME
#undef IGDARRAY_LINKAGE
#undef IGDARRAY_push
#ifdef IGDARRAY_DECLS_ONLY
#undef IGDARRAY_DECLS_ONLY
#endif
