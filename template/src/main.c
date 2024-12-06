#include <stdlib.h>
#include "log.h"
#include "dynamic_string.h"
#include "file_handler.h"

int main()
{
    init_logger(LOG_LEVEL_TRACE);

    IGFile *file = igOpenFileByConst("input.txt", "r");

    log_message(LOG_LEVEL_TRACE, "Line count: %d", file->content.size);

    for (int i = 0; i < file->content.size; i++)
    {
        log_message(LOG_LEVEL_TRACE, "Content:  %str ---> Line %d", file->content.data[i], i);
    }

    igCloseFile(file);

    return 0;
}