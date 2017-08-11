//Pawel Grochola
#ifndef LIST_H
#define LIST_H
#include "comparators.h"

typedef struct ABEntry ABEntry;

typedef struct ABListNode
{
    ABEntry* entry;
    struct ABListNode* prev;
    struct ABListNode* next;
} ABListNode;

typedef struct ABList
{
    ABListNode* head;
    ABListNode* tail;
} ABList;

ABList* ab_list_new();
void ab_list_delete(ABList* list);
void ab_list_insert_sorted(ABList* list, ABListNode* to_insert, ABComparatorType comparator);
void ab_list_insert_afterhead(ABList* list, ABEntry* to_insert);
ABList* ab_list_copy_all_equal(ABList* list, ABEntry* equal_to, ABComparatorType comparator);
ABEntry* ab_list_find_any(ABList* list, ABEntry* equal_to, ABComparatorType comparator);
bool ab_list_delete_first_match(ABList* list, ABEntry* match_to, ABComparatorType comparator);
void ab_list_sort(ABList* list, ABComparatorType comparator);

ABListNode* ab_listnode_new(ABEntry* const entry);

#endif
