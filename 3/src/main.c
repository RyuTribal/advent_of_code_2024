#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int isNextMul(FILE *ptr)
{
    char buffer[3];
    int i;
    for (i = 0; i < 3; i++)
    {
        buffer[i] = fgetc(ptr);
        if (buffer[i] == EOF)
        {
            for (int j = i - 1; j >= 0; j--)
                ungetc(buffer[j], ptr);
            return 0;
        }
    }
    if (buffer[0] == 'm' && buffer[1] == 'u' && buffer[2] == 'l')
    {
        for (i = 2; i >= 0; i--)
            ungetc(buffer[i], ptr);
        return 1;
    }
    for (i = 2; i >= 0; i--)
        ungetc(buffer[i], ptr);

    return 0;
}

// 1: yes
// 0: invalid
// -1: no
int isNextDo(FILE *ptr)
{
    char buffer[4];
    int i;
    for (i = 0; i < 4; i++)
    {
        buffer[i] = fgetc(ptr);
        if (buffer[i] == EOF)
        {
            for (int j = i - 1; j >= 0; j--)
                ungetc(buffer[j], ptr);
            return 0;
        }
    }
    if (buffer[0] == 'd' && buffer[1] == 'o' && buffer[2] == '(' && buffer[3] == ')')
    {
        for (i = 3; i >= 0; i--)
            ungetc(buffer[i], ptr);
        return 1;
    }

    for (i = 3; i >= 0; i--)
        ungetc(buffer[i], ptr);

    return 0;
}

// 1: yes
// 0: invalid
// -1: no
int isNextDont(FILE *ptr)
{
    char buffer[7];
    int i;
    for (i = 0; i < 7; i++)
    {
        buffer[i] = fgetc(ptr);
        if (buffer[i] == EOF)
        {
            for (int j = i - 1; j >= 0; j--)
                ungetc(buffer[j], ptr);
            return 0;
        }
    }
    if (buffer[0] == 'd' && buffer[1] == 'o' && buffer[2] == 'n' && buffer[3] == '\'' &&
        buffer[4] == 't' && buffer[5] == '(' && buffer[6] == ')')
    {
        for (i = 6; i >= 0; i--)
            ungetc(buffer[i], ptr);
        return 1;
    }

    for (i = 6; i >= 0; i--)
        ungetc(buffer[i], ptr);

    return 0;
}

int parseUntilNextMul(FILE *ptr, int *allow_mul)
{
    char ch;
    while (1)
    {
        if (isNextMul(ptr) == 1)
        {
            fgetc(ptr);
            fgetc(ptr);
            fgetc(ptr);
            return 1;
        }
        else if (isNextDo(ptr) == 1)
        {
            printf("Next is do \n");
            fgetc(ptr);
            fgetc(ptr);
            fgetc(ptr);
            fgetc(ptr);
            *allow_mul = 1;
        }
        else if (isNextDont(ptr) == 1)
        {
            printf("Next is dont \n");
            fgetc(ptr);
            fgetc(ptr);
            fgetc(ptr);
            fgetc(ptr);
            fgetc(ptr);
            fgetc(ptr);
            fgetc(ptr);
            *allow_mul = 0;
        }
        else
        {
            ch = fgetc(ptr);
            if (ch == EOF)
            {
                return 0;
            }
            printf("current character %c\n", ch);
        }
    }
}

int parseArgs(FILE *ptr, char *args_string, int *separator_index)
{
    int characters_counted = 0;
    int found_separator = 0;

    char ch = fgetc(ptr);
    if (ch != '(')
    {
        ungetc(ch, ptr);
        return 0;
    }

    while (1)
    {
        ch = fgetc(ptr);
        if (ch == EOF)
            return -1;

        if (ch == ')')
            break;

        if (ch == ',')
        {
            if (found_separator)
                return 0;
            *separator_index = characters_counted;
            found_separator = 1;
        }
        else if (!isdigit(ch))
        {
            ungetc(ch, ptr);
            return 0;
        }

        if (characters_counted >= 9)
            return 0;
        args_string[characters_counted++] = ch;
    }

    args_string[characters_counted] = '\0';
    return found_separator ? 1 : 0;
}

void parseArgumentNumbers(int *nr1, int *nr2, char *args_string, int separator_index)
{
    char *first_part = (char *)malloc(separator_index + 1);
    if (!first_part)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    strncpy(first_part, args_string, separator_index);
    first_part[separator_index] = '\0';

    *nr1 = atoi(first_part);
    *nr2 = atoi(&args_string[separator_index + 1]);

    free(first_part);
}

int main()
{
    FILE *ptr;

    ptr = fopen("puzzle_input.txt", "r");

    if (ptr == NULL)
    {
        printf("File can't be opened \n");
        return 1;
    }

    int has_mul = 1;
    int allows_mul = 1;
    int mul_count = 0;
    int has_valid_args = 1;
    int valid_args_count = 0;
    int separator_index;

    char args_string[8];

    int nr1;
    int nr2;
    int final_result = 0;
    do
    {
        has_mul = parseUntilNextMul(ptr, &allows_mul);
        if (has_mul == 1)
        {
            mul_count++;
            if (allows_mul == 1)
            {
                has_valid_args = parseArgs(ptr, args_string, &separator_index);
                if (has_valid_args == 1)
                {
                    valid_args_count++;
                    parseArgumentNumbers(&nr1, &nr2, args_string, separator_index);
                    final_result += nr1 * nr2;
                }
            }
        }

    } while (has_mul == 1);

    printf("Mul count is: %d\n", mul_count);
    printf("Valid args count is: %d\n", valid_args_count);
    printf("Final result is: %d\n", final_result);

    fclose(ptr);
    return 0;
}