#include <stdlib.h>
#include "log.h"
#include "dynamic_string.h"
#include "file_handler.h"
#include "ig_enums.h"

#define IGDARRAY_T IGLines *
#define IGDARRAY_PREFIX rules_
#define IGDARRAY_NAME Rules

#include "dynamic_array.h"

#define IGDARRAY_T IGLines *
#define IGDARRAY_PREFIX updates_
#define IGDARRAY_NAME Updates

#include "dynamic_array.h"

#define IGHMAP_KEY_T IGString
#define IGHMAP_VALUE_T IGLines *
#define IGHMAP_PREFIX relationship_
#define IGHMAP_NAME Relationships

#include "ig_hashmap.h"

int main()
{
    init_logger(LOG_LEVEL_TRACE);

    IGFile *file = igOpenFileByConst("input.txt", "r");

    log_message(LOG_LEVEL_TRACE, "Line count: %d", file->content.size);

    IGLines rules = {NULL, 0, 0};
    IGLines updates = {NULL, 0, 0};
    IGBool has_rules = IGFalse;
    for (int i = 0; i < file->content.size; i++)
    {
        if (!file->content.data[i].data || isdigit(file->content.data[i].data[0]) == 0)
        {
            has_rules = IGTrue;
            continue;
        }

        if (has_rules)
        {

            igLines_push(&updates, file->content.data[i]);
        }
        else
        {
            igLines_push(&rules, file->content.data[i]);
        }
    }

    Rules xmas_rules = {NULL, 0, 0};

    for (int i = 0; i < rules.size; i++)
    {
        rules_push(&xmas_rules, igStringSplit(&rules.data[i], '|'));
    }

    Updates xmas_updates = {NULL, 0, 0};

    for (int i = 0; i < updates.size; i++)
    {
        updates_push(&xmas_updates, igStringSplit(&updates.data[i], ','));
    }
    Relationships *xmas_rule_relations = relationship_init(igStringHash, areStringsEqual);
    for (int i = 0; i < xmas_rules.size; i++)
    {
        if (relationship_get(xmas_rule_relations, xmas_rules.data[i]->data[0]))
        {
            continue;
        }
        relationship_insert(xmas_rule_relations, string_copy(&xmas_rules.data[i]->data[0]), igLines_init());
        for (int j = 0; j < xmas_rules.size; j++)
        {
            if (areStringsEqual(&xmas_rules.data[j]->data[0], &xmas_rules.data[i]->data[0]))
            {
                igLines_push(relationship_get(xmas_rule_relations, xmas_rules.data[i]->data[0]), string_copy(&xmas_rules.data[j]->data[1]));
            }
        }
    }

    Updates correct_updates = {NULL, 0, 0};

    for (int i = 0; i < xmas_updates.size; i++)
    {
        IGBool is_correct = IGTrue;
        for (int j = xmas_updates.data[i]->size - 1; j >= 0; j--)
        {
            IGLines *after_rules = relationship_get(xmas_rule_relations, xmas_updates.data[i]->data[j]);
            if (!after_rules)
            {
                continue;
            }
            for (int subsequent = j; subsequent >= 0; subsequent--)
            {
                for (int rule = 0; rule < after_rules->size; rule++)
                {
                    if (areStringsEqual(&after_rules->data[rule], &xmas_updates.data[i]->data[subsequent]))
                    {
                        is_correct = IGFalse;
                        break;
                    }
                }
                if (!is_correct)
                {
                    break;
                }
            }
            if (!is_correct)
            {
                break;
            }
        }
        if (is_correct)
        {
            updates_push(&correct_updates, xmas_updates.data[i]);
        }
    }

    int total_result = 0;
    for (int i = 0; i < correct_updates.size; i++)
    {
        int middle = correct_updates.data[i]->size / 2;
        int nr = atoi(correct_updates.data[i]->data[middle].data);
        total_result += atoi(correct_updates.data[i]->data[middle].data);
    }

    log_message(LOG_LEVEL_INFO, "Result: %d", total_result);

    for (int i = 0; i < xmas_rules.size; i++)
    {
        igDestroyLines(xmas_rules.data[i]);
    }

    for (int i = 0; i < xmas_updates.size; i++)
    {
        igDestroyLines(xmas_updates.data[i]);
    }

    relationship_destroy(xmas_rule_relations);

    igCloseFile(file);

    return 0;
}