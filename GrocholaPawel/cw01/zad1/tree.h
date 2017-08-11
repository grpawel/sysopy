//Pawel Grochola
#ifndef TREE_H
#define TREE_H
#include "comparators.h"
#include "linked_list.h"


typedef struct ABEntry ABEntry;

typedef struct ABTreeNode
{
    ABEntry* entry;
    struct ABTreeNode* left;
    struct ABTreeNode* right;
    struct ABTreeNode* parent;
} ABTreeNode;


typedef struct ABTree
{
    ABTreeNode* root;
} ABTree;

ABTree* ab_tree_new();
void ab_tree_delete(ABTree* const tree);
bool ab_tree_delete_first_match(ABTree* tree, ABEntry* to_remove, ABComparatorType comparator);
void ab_tree_insert(ABTree* const tree, ABEntry* to_insert, ABComparatorType comparator);
void ab_tree_rebuild(ABTree* tree, ABComparatorType comparator);
ABEntry* ab_tree_search(ABTree* tree, ABEntry* equal_to, ABComparatorType comparator_tree, ABComparatorType comparator_value);
ABList* ab_tree_find_all(ABTree* tree, ABEntry* equal_to, ABComparatorType comparator);
#endif
