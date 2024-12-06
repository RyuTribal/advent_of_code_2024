#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_HANDLERS 16

static LoggerConfig log_config = {LOG_LEVEL_TRACE};

typedef struct
{
    const char *format_id;
    LogHandler handler;
} LogHandlerRegistry;

static LogHandlerRegistry handler_registry[MAX_HANDLERS];
static size_t handler_count = 0;

static void get_timestamp(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t == NULL)
    {
        snprintf(buffer, size, "Unknown Time");
        return;
    }
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

uint8_t init_logger(LogLevel level)
{
    log_config.level = level;
    return 1;
}

void set_logger_level(LogLevel level)
{
    log_config.level = level;
}

void register_log_handler(const char *format_id, LogHandler handler)
{
    if (handler_count >= MAX_HANDLERS)
    {
        fprintf(stderr, "Maximum number of log handlers reached\n");
        return;
    }
    handler_registry[handler_count].format_id = format_id;
    handler_registry[handler_count].handler = handler;
    handler_count++;
}

void handle_general_formatters(char formatter_char, va_list *args)
{
    switch (formatter_char)
    {
    case 'd':
    case 'i':
    {
        int integer = va_arg(*args, int);
        printf("%d", integer);
        break;
    }
    case 'u':
    {
        unsigned int unsigned_integer = va_arg(*args, unsigned int);
        printf("%u", unsigned_integer);
        break;
    }
    case 'o':
    {
        unsigned int octal = va_arg(*args, unsigned int);
        printf("%o", octal);
        break;
    }
    case 'x':
    {
        unsigned int hex = va_arg(*args, unsigned int);
        printf("%x", hex);
        break;
    }
    case 'X':
    {
        unsigned int hex_upper = va_arg(*args, unsigned int);
        printf("%X", hex_upper);
        break;
    }
    case 'f':
    case 'F':
    {
        double floating_point = va_arg(*args, double);
        printf("%f", floating_point);
        break;
    }
    case 'e':
    {
        double scientific = va_arg(*args, double);
        printf("%e", scientific);
        break;
    }
    case 'E':
    {
        double scientific_upper = va_arg(*args, double);
        printf("%E", scientific_upper);
        break;
    }
    case 'g':
    {
        double shortest = va_arg(*args, double);
        printf("%g", shortest);
        break;
    }
    case 'G':
    {
        double shortest_upper = va_arg(*args, double);
        printf("%G", shortest_upper);
        break;
    }
    case 'a':
    {
        double hex_float = va_arg(*args, double);
        printf("%a", hex_float);
        break;
    }
    case 'A':
    {
        double hex_float_upper = va_arg(*args, double);
        printf("%A", hex_float_upper);
        break;
    }
    case 'c':
    {
        int character = va_arg(*args, int);
        printf("%c", character);
        break;
    }
    case 's':
    {
        char *string = va_arg(*args, char *);
        printf("%s", string);
        break;
    }
    case 'p':
    {
        void *pointer = va_arg(*args, void *);
        printf("%p", pointer);
        break;
    }
    case 'n':
    {
        int *char_count_ptr = va_arg(*args, int *);
        *char_count_ptr = 0;
        break;
    }
    case '%':
    {
        printf("%%");
        break;
    }
    default:
        printf("%%%c", formatter_char);
        break;
    }
}

void log_message(LogLevel level, const char *format, ...)
{
    if (level < log_config.level)
    {
        return;
    }

    char timestamp[20];
    get_timestamp(timestamp, sizeof(timestamp));

    const char *color;
    const char *level_str;

    switch (level)
    {
    case LOG_LEVEL_TRACE:
        color = BLUE;
        level_str = "TRACE";
        break;
    case LOG_LEVEL_INFO:
        color = GREEN;
        level_str = "INFO";
        break;
    case LOG_LEVEL_WARN:
        color = YELLOW;
        level_str = "WARNING";
        break;
    case LOG_LEVEL_ERROR:
        color = RED;
        level_str = "ERROR";
        break;
    default:
        color = RESET;
        level_str = "UNKNOWN";
    }
    printf("[%s] %s[%s] ", timestamp, color, level_str);

    va_list args;
    va_start(args, format);

    while (*format)
    {
        if (*format == '%')
        {
            format++;
            int handled = 0;

            for (size_t i = 0; i < handler_count; i++)
            {
                size_t id_len = strlen(handler_registry[i].format_id);
                if (strncmp(format, handler_registry[i].format_id, id_len) == 0)
                {
                    handler_registry[i].handler(&args);
                    format += id_len;
                    handled = 1;
                    break;
                }
            }
            if (!handled)
            {
                handle_general_formatters(*format, &args);
                format++;
            }
        }
        else
        {
            putchar(*format);
            format++;
        }
    }

    va_end(args);

    printf(RESET "\n");
}
