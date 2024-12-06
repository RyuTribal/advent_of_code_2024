#include "file_handler.h"

IGFile *igOpenFileByConst(char *file_name, char *mode)
{
    IGString *new_str = igCreateStringByConst(file_name);

    IGFile *file = igOpenFile(new_str, mode);
    igDestroyString(new_str);

    return file;
}
IGFile *igOpenFile(IGString *file_name, char *mode)
{
    IGFile *file = malloc(sizeof(IGFile));
    file->ptr = fopen(file_name->data, mode);
    if (file->ptr == NULL)
    {
        log_message(LOG_LEVEL_ERROR, "Couldn't open file");
        exit(EXIT_FAILURE);
    }

    file->content.data = NULL;
    file->content.size = 0;
    file->content.capacity = 0;

    char ch;

    IGString empty_str = {NULL, 0, 0};

    igLines_push(&file->content, string_copy(&empty_str));
    while ((ch = fgetc(file->ptr)) != EOF)
    {
        if (ch == '\n')
        {
            igLines_push(&file->content, string_copy(&empty_str));
            continue;
        }
        igAppendCharToString(&file->content.data[file->content.size - 1], ch);
    }

    fclose(file->ptr);
    return file;
}

void igCloseFile(IGFile *file)
{
    igDestroyLines(&file->content);
    free(file);
}