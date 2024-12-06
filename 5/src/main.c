#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAGES 100
#define BITMAP_SIZE (MAX_PAGES / (sizeof(uint64_t) * 8) + 1)

uint64_t rules_bitmap[MAX_PAGES];

void parse_rules(char **rules, int rules_count)
{
    memset(rules_bitmap, 0, sizeof(rules_bitmap));
    for (int i = 0; i < rules_count; i++)
    {
        int before, after;
        sscanf(rules[i], "%d|%d", &before, &after);
        rules_bitmap[before] |= 1ULL << after;
    }
}

int validate_line(int *line, int line_size)
{
    for (int i = 0; i < line_size - 1; i++)
    {
        if ((rules_bitmap[line[i]] & (1ULL << line[i + 1])) == 0)
        {
            return 0;
        }
    }
    return 1;
}

int compare_pages(const void *a, const void *b)
{
    int page_a = *(const int *)a;
    int page_b = *(const int *)b;

    if ((rules_bitmap[page_a] & (1ULL << page_b)) != 0)
    {
        return -1;
    }
    else if ((rules_bitmap[page_b] & (1ULL << page_a)) != 0)
    {
        return 1;
    }
    return 0;
}

void sort_line(int *line, int line_size)
{
    qsort(line, line_size, sizeof(int), compare_pages);
}

void process_updates(int **updates, int updates_count, int *update_sizes, int *valid_sum, int *invalid_sum)
{
    *valid_sum = 0;
    *invalid_sum = 0;

    for (int i = 0; i < updates_count; i++)
    {
        if (validate_line(updates[i], update_sizes[i]))
        {
            *valid_sum += updates[i][update_sizes[i] / 2];
        }
        else
        {
            sort_line(updates[i], update_sizes[i]);
            *invalid_sum += updates[i][update_sizes[i] / 2];
        }
    }
}
