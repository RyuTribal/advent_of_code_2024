#include <stdio.h>
#include <stdlib.h>
#include "log.h"

// 0 = no error
// 1 = error
int getRowsAndLongestLine(char *file_name, int *rows, int *longest_line)
{
    FILE *fptr;
    char ch;

    fptr = fopen(file_name, "r");

    if (fptr == NULL)
    {
        log_message(LOG_LEVEL_ERROR, "Couldn't open file");
        return 1;
    }

    int current_line = 0;

    while ((ch = fgetc(fptr)) != EOF)
    {
        if (ch == '\n')
        {
            *rows += 1;
            current_line = 0;
        }
        else
        {
            current_line++;
        }

        if (current_line > *longest_line)
        {
            *longest_line = current_line;
            current_line = 0;
        }
    }
    *longest_line += 1;

    fclose(fptr);
    return 0;
}

// returns occurances
int checkVertical(char *buffer, int x_index, int rows, int columns)
{
    int occurrences = 0;

    if ((x_index / columns) + 3 < rows)
    {
        if (buffer[x_index + columns] == 'M' &&
            buffer[x_index + 2 * columns] == 'A' &&
            buffer[x_index + 3 * columns] == 'S')
        {
            occurrences++;
        }
    }

    if ((x_index / columns) - 3 >= 0)
    {
        if (buffer[x_index - columns] == 'M' &&
            buffer[x_index - 2 * columns] == 'A' &&
            buffer[x_index - 3 * columns] == 'S')
        {
            occurrences++;
        }
    }

    return occurrences;
}

int checkHorizontal(char *buffer, int x_index, int rows, int columns)
{
    int occurrences = 0;
    if ((x_index % columns) + 3 < columns)
    {
        if (buffer[x_index + 1] == 'M' &&
            buffer[x_index + 2] == 'A' &&
            buffer[x_index + 3] == 'S')
        {
            occurrences++;
        }
    }

    if ((x_index % columns) - 3 >= 0)
    {
        if (buffer[x_index - 1] == 'M' &&
            buffer[x_index - 2] == 'A' &&
            buffer[x_index - 3] == 'S')
        {
            occurrences++;
        }
    }

    return occurrences;
}

int checkDiagonal(char *buffer, int x_index, int rows, int columns)
{
    int occurrences = 0;

    // Top-Left to Bottom-Right (↘)
    if ((x_index / columns) + 3 < rows && (x_index % columns) + 3 < columns)
    {
        if (buffer[x_index + columns + 1] == 'M' &&
            buffer[x_index + 2 * (columns + 1)] == 'A' &&
            buffer[x_index + 3 * (columns + 1)] == 'S')
        {
            occurrences++;
        }
    }

    // Top-Right to Bottom-Left (↙)
    if ((x_index / columns) + 3 < rows && (x_index % columns) - 3 >= 0)
    {
        if (buffer[x_index + columns - 1] == 'M' &&
            buffer[x_index + 2 * (columns - 1)] == 'A' &&
            buffer[x_index + 3 * (columns - 1)] == 'S')
        {
            occurrences++;
        }
    }

    // Bottom-Left to Top-Right (↗)
    if ((x_index / columns) - 3 >= 0 && (x_index % columns) + 3 < columns)
    {
        if (buffer[x_index - columns + 1] == 'M' &&
            buffer[x_index - 2 * (columns - 1)] == 'A' &&
            buffer[x_index - 3 * (columns - 1)] == 'S')
        {
            occurrences++;
        }
    }

    // Bottom-Right to Top-Left (↖)
    if ((x_index / columns) - 3 >= 0 && (x_index % columns) - 3 >= 0)
    {
        if (buffer[x_index - columns - 1] == 'M' &&
            buffer[x_index - 2 * (columns + 1)] == 'A' &&
            buffer[x_index - 3 * (columns + 1)] == 'S')
        {
            occurrences++;
        }
    }

    return occurrences;
}

int checkXmasOccurance(char *buffer, int x_index, int rows, int columns)
{
    int found_line_1 = 0;
    int found_line_2 = 0;
    char m_char = 'M';
    char s_char = 'S';
    char found_char;
    if ((x_index / columns) - 1 >= 0 && (x_index / columns) + 1 < rows)
    {
        if (buffer[x_index + columns + 1] == 'M' || buffer[x_index + columns + 1] == 'S')
        {
            found_char = buffer[x_index + columns + 1];
            if (found_char == m_char && buffer[x_index - columns - 1] == 'S')
            {
                found_line_1 = 1;
            }
            else if (found_char == s_char && buffer[x_index - columns - 1] == 'M')
            {
                found_line_1 = 1;
            }
        }

        if (buffer[x_index - columns + 1] == 'M' || buffer[x_index - columns + 1] == 'S')
        {
            found_char = buffer[x_index - columns + 1];
            if (found_char == m_char && buffer[x_index + columns - 1] == 'S')
            {
                found_line_2 = 1;
            }
            else if (found_char == s_char && buffer[x_index + columns - 1] == 'M')
            {
                found_line_2 = 1;
            }
        }
    }

    return found_line_1 && found_line_2 ? 1 : 0;
}

int main()
{
    init_logger(LOG_LEVEL_TRACE);
    char *file_name = "input.txt";

    int rows = 1;
    int longest_line = 0;

    if (getRowsAndLongestLine(file_name, &rows, &longest_line) != 0)
    {
        log_message(LOG_LEVEL_ERROR, "Couldn't open file");
        return 1;
    }

    FILE *fptr;
    fptr = fopen(file_name, "r");

    if (fptr == NULL)
    {
        log_message(LOG_LEVEL_ERROR, "Couldn't open file");
        return 1;
    }

    char *lines = (char *)malloc(rows * longest_line);

    char ch;

    log_message(LOG_LEVEL_INFO, "Rows: %d", rows);
    log_message(LOG_LEVEL_INFO, "Longest line length: %d\n", longest_line);

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < longest_line; col++)
        {
            if (col == longest_line - 1 && row == rows - 1)
            {
                break;
            }
            lines[row * longest_line + col] = fgetc(fptr);
        }
    }

    lines[rows * longest_line - 1] = '\n';

    int xmas_occurance = 0;

    for (int i = 0; i < rows * longest_line; i++)
    {
        if (lines[i] == 'A')
        {
            // xmas_occurance += checkVertical(lines, i, rows, longest_line);
            // xmas_occurance += checkHorizontal(lines, i, rows, longest_line);
            // xmas_occurance += checkDiagonal(lines, i, rows, longest_line);
            xmas_occurance += checkXmasOccurance(lines, i, rows, longest_line);
        }
    }

    log_message(LOG_LEVEL_INFO, "Occurances of X-MAS: %d", xmas_occurance);

    fclose(fptr);

    return 0;
}