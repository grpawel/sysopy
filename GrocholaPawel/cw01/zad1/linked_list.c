//Pawel Grochola
#include "linked_list.h"
#include <stdlib.h>
#include "address_book.h"

static void ab_listnode_connect(ABListNode* first, ABListNode* second)
{
    first->next = second;
    second->prev = first;
}

ABListNode* ab_listnode_new(ABEntry* const entry)
{
    ABListNode* node = malloc(sizeof(ABListNode));
    node->entry = entry;
    return node;
}

ABList* ab_list_new()
{
    ABList* list = malloc(sizeof(ABList));
    list->head = ab_listnode_new(NULL);
    list->tail = ab_listnode_new(NULL);
    ab_listnode_connect(list->head, list->tail);
    return list;
}

static void ab_listnode_insert_after(ABListNode* before, ABListNode* to_insert)
{
    ab_listnode_connect(to_insert, before->next);
    ab_listnode_connect(before, to_insert);
}

static void ab_listnode_insert_before(ABListNode* after, ABListNode* to_insert)
{
    ab_listnode_connect(after->prev, to_insert);
    ab_listnode_connect(to_insert, after);
}
void ab_list_insert_afterhead(ABList* list, ABEntry* to_insert)
{
    ab_listnode_insert_after(list->head, ab_listnode_new(to_insert));
}

void ab_list_insert_sorted(ABList* list, ABListNode* to_insert, ABComparatorType comparator)
{
    ABListNode* current = list->head;
    do {
        current = current->next;
    } while(current != list->tail && comparator(current->entry, to_insert->entry) < 0);
    ab_listnode_insert_before(current, to_insert);
}

//returns NULL if list is empty
static ABListNode* ab_list_find_max(ABList* list, ABComparatorType comparator)
{
    ABListNode* max = list->head->next;
    if(max == list->tail) {
        return NULL;
    }
    ABListNode* candidate = max->next;
    while(candidate != list->tail) {
        if(comparator(candidate->entry, max->entry) > 0) {
            max = candidate;
        }
        candidate = candidate->next;
    }
    return max;
}

static void ab_listnode_cut(ABListNode* node)
{
    ab_listnode_connect(node->prev, node->next);
    node->prev = node->next = NULL;
}

void ab_list_sort(ABList* list, ABComparatorType comparator)
{
    ABListNode* new_head = ab_listnode_new(NULL);
    ABListNode* new_tail = ab_listnode_new(NULL);
    ab_listnode_connect(new_head, new_tail);
    while(list->head->next != list->tail) {
        ABListNode* max = ab_list_find_max(list, comparator);
        ab_listnode_cut(max);
        ab_listnode_insert_after(new_head, max);
    }
    free(list->head);
    free(list->tail);
    list->head = new_head;
    list->tail = new_tail;
}

static ABListNode* ab_listnode_copy(ABListNode* node)
{
    return ab_listnode_new(ab_entry_copy(node->entry));
}

ABList* ab_list_copy_all_equal(ABList* list, ABEntry* equal_to, ABComparatorType comparator)
{
    ABList* result = ab_list_new();
    ABListNode* current = list->head->next;
    while(current != list->tail) {
        if(comparator(current->entry, equal_to) == 0) {
            ab_listnode_insert_before(result->tail, ab_listnode_copy(current));
        }
        current = current->next;
    }
    return result;
}

ABEntry* ab_list_find_any(ABList* list, ABEntry* equal_to, ABComparatorType comparator)
{
    ABListNode* current = list->head->next;
    while(current != list->tail && comparator(current->entry, equal_to) != 0) {
        current = current->next;
    }
    if(current != list->tail) {
        return current->entry;
    }
    else {
        return NULL;
    }
}

// deletes both node and associated entry
static void ab_listnode_delete(ABListNode* node)
{
    if(node == NULL) return;
    node->next->prev = node->prev;
    node->prev->next = node->next;
    if(node->entry != NULL) {
        free(node->entry);
    }
    free(node);
}

bool ab_list_delete_first_match(ABList* list, ABEntry* match_to, ABComparatorType comparator)
{
    ABListNode* current = list->head->next;
    while(current != list->tail && comparator(current->entry, match_to) != 0) {
        current = current->next;
    }
    if(current != list->tail) {
        ab_listnode_delete(current);
        return true;
    }
    else {
        return false;
    }
}

void ab_list_delete(ABList* list)
{
    if(list == NULL) return;
    //delete all nodes with entries
    ABListNode* current_node = list->head->next;
    while(current_node != list->tail) {
        if(current_node->entry != NULL) {
            free(current_node->entry);
        }
        current_node = current_node->next;
        free(current_node->prev);
    }
    //delete guards (these should not have associated entries)
    free(list->head);
    free(list->tail);
    free(list);
}
