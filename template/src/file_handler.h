#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "ig_enums.h"
#include "dynamic_string.h"

typedef struct
{
    IGLines content;
    FILE *ptr;
} IGFile;

IGFile *igOpenFile(IGString *file_name, char *mode);
IGFile *igOpenFileByConst(char *file_name, char *mode);
void igCloseFile(IGFile *file);
