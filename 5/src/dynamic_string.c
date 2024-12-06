#include "dynamic_string.h"

static IGBool ig_str_registered_logger = IGFalse;

size_t igStringHash(IGString *key, size_t capacity)
{
    const char *str = key->data;
    size_t hash = 5381;
    while (*str)
    {
        hash = ((hash << 5) + hash) + (unsigned char)(*str); // hash * 33 + c
        str++;
    }
    return hash % capacity;
}

void log_igstring_handler(va_list *args)
{
    IGString *str = va_arg(*args, IGString *);
    for (size_t i = 0; i < str->size; i++)
    {
        putchar(str->data[i]);
    }
}

void ig_init_str_globals()
{
    if (ig_str_registered_logger == IGFalse)
    {
        register_log_handler("str", log_igstring_handler);
        ig_str_registered_logger = IGTrue;
    }
}

// Has to be null terminated
IGString *igCreateStringByConst(char *initial_str)
{
    ig_init_str_globals();

    int i = 0;
    while (initial_str[i] != '\0')
    {
        i++;
    }
    return igCreateString(initial_str, i);
}

// Doesnt have to be null terminated
IGString *igCreateString(char *initial_str, size_t string_size)
{
    IGString *str = string_init();
    if (!str)
    {
        log_message(LOG_LEVEL_ERROR, "Could not allocate memory for the string!");
        exit(EXIT_FAILURE);
    }

    if (string_size < 0)
    {
        log_message(LOG_LEVEL_ERROR, "Can not create a string with a size lower than 0!");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < string_size; i++)
    {
        string_push(str, initial_str[i]);
    }

    string_push(str, '\0');
    return str;
}

void igDestroyString(IGString *str)
{
    string_destroy(str);
}

void igAddStrings(IGString *dest, const IGString *src)
{
    size_t new_size = dest->size + src->size;
    if (new_size > dest->capacity)
    {
        dest->capacity = new_size + 10;
        dest->data = realloc(dest->data, dest->capacity);
        if (!dest->data)
        {
            log_message(LOG_LEVEL_ERROR, "Failed to allocate memory for IGString concatenation");
            exit(EXIT_FAILURE);
        }
    }
    memcpy(&dest->data[dest->size - 1], src->data, src->size);
    dest->size = new_size;
    dest->data[dest->size - 1] = '\0';
}

void igAppendCharToString(IGString *dest, char src)
{
    if (dest->size == 0)
    {
        string_push(dest, src);
    }
    else
    {
        dest->data[dest->size - 1] = src;
    }

    if (dest->size + 1 > dest->capacity)
    {
        dest->capacity = dest->size + 10;
        dest->data = realloc(dest->data, dest->capacity);
        if (!dest->data)
        {
            log_message(LOG_LEVEL_ERROR, "Failed to allocate memory for IGString concatenation");
            exit(EXIT_FAILURE);
        }
    }
    dest->data[dest->size] = '\0';
    dest->size += 1;
}

void igAddStringsByConst(IGString *dest, char *src)
{
    IGString *new_str = igCreateStringByConst(src);

    igAddStrings(dest, new_str);

    igDestroyString(new_str);
}

IGLines *igStringSplit(IGString *str, char delimiter)
{
    IGLines *lines = igLines_init();

    IGString curr_str = {NULL, 0, 0};
    string_push(&curr_str, '\0');

    for (int i = 0; i < str->size; i++)
    {
        if (str->data[i] == delimiter)
        {
            igLines_push(lines, string_copy(&curr_str));
            curr_str.data = NULL;
            curr_str.capacity = 0;
            curr_str.size = 0;
            string_push(&curr_str, '\0');
            continue;
        }
        else if (isdigit(str->data[i]))
        {
            igAppendCharToString(&curr_str, str->data[i]);
        }
    }

    igLines_push(lines, string_copy(&curr_str));

    return lines;
}

IGBool areStringsEqual(IGString *str1, IGString *str2)
{
    if (str1->size != str2->size)
    {
        return IGFalse;
    }
    for (int i = 0; i < str1->size; i++)
    {
        if (str1->data[i] != str2->data[i])
        {
            return IGFalse;
        }
    }

    return IGTrue;
}

void igDestroyLines(IGLines *lines)
{
    for (int i = 0; i < lines->size; i++)
    {
        string_destroy(&lines->data[i]);
    }
    igLines_destroy(lines);
}