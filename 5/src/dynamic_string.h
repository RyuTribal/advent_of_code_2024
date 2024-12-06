#pragma once
#include <stdlib.h>
#include "log.h"
#include "ig_enums.h"

// Dynamic char array
#define IGDARRAY_T char
#define IGDARRAY_PREFIX string_
#define IGDARRAY_NAME IGString

#include "dynamic_array.h"

// Dynamic string array
#define IGDARRAY_T IGString
#define IGDARRAY_PREFIX igLines_
#define IGDARRAY_NAME IGLines

#include "dynamic_array.h"

size_t igStringHash(IGString *key, size_t capacity);

IGString *igCreateString(char *initial_str, size_t string_size);
IGString *igCreateStringByConst(char *initial_str);
void igDestroyString(IGString *str);
void igAddStrings(IGString *dest, const IGString *src);
void igAddStringsByConst(IGString *dest, char *src);
void igAppendCharToString(IGString *dest, char src);
IGLines *igStringSplit(IGString *str, char delimiter);
IGBool areStringsEqual(IGString *str1, IGString *str2);

void igDestroyLines(IGLines *lines);