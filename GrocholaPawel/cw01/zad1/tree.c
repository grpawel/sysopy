//Pawel Grochola
#include "tree.h"
#include <stdlib.h>

static void ab_treenode_delete_all(ABTreeNode* const node);

ABTree* ab_tree_new()
{
    ABTree* tree = malloc(sizeof(tree));
    tree->root = NULL;
    return tree;
}

void ab_tree_delete(ABTree* const tree)
{
    ab_treenode_delete_all(tree->root);
}

ABTreeNode* ab_treenode_new(ABEntry* entry)
{
    ABTreeNode* node = malloc(sizeof(ABTreeNode));
    node->entry = entry;
    node->parent = node->left = node->right = NULL;
    return node;
}


static void ab_treenode_delete_all(ABTreeNode* const node)
{
    if(node != NULL) {
        ab_treenode_delete_all(node->left);
        ab_treenode_delete_all(node->right);
        ab_entry_delete(node->entry);
        free(node);
    }
}

void ab_tree_insert_node(ABTree* const tree, ABTreeNode* node, ABComparatorType comparator)
{
    ABTreeNode* current = tree->root;
    while (1) {
        if(comparator(node->entry, current->entry) <= 0) {
            if(current->left == NULL)
                break;
            current = current->left;
        }
        else {
            if(current->right == NULL)
                break;
            current = current->right;
        }
    }
    node->parent = current;
    if(comparator(node->entry, current->entry) <= 0)
        current->left = node;
    else
        current->right = node;
}

static ABTreeNode* ab_tree_min(ABTreeNode* node)
{
    while(node->left != NULL)
        node = node -> left;
    return node;
}

static ABTreeNode* ab_tree_max(ABTreeNode* node)
{
    while(node->right != NULL)
        node = node->right;
    return node;
}

static ABTreeNode* ab_tree_successor(ABTreeNode* prev)
{
    if(prev->right != NULL) {
        return ab_tree_min(prev->right);
    }
    ABTreeNode* parent = prev->parent;
    while(parent != NULL && prev == parent->right) {
        prev = parent;
        parent = parent->parent;
    }
    return parent;
}
/*
static ABTreeNode* ab_tree_predecessor(ABTreeNode* next)
{
    if(next->left != NULL) {
        return ab_tree_max(next->left);
    }
    ABTreeNode* parent = next->parent;
    while(parent != NULL && next == parent->left) {
        next = parent;
        parent = parent->parent;
    }
    return parent;
}
*/
static void ab_treenode_insert(ABTreeNode* root, ABTreeNode* to_insert, ABComparatorType comparator)
{
    if(comparator(root->entry, to_insert->entry) <= 0) {
        if(root->left == NULL) {
            root->left = to_insert;
            to_insert->parent = root;
        }
        else {
            ab_treenode_insert(root->left, to_insert, comparator);
        }
    }
    else {
        if(root->right == NULL) {
            root->right = to_insert;
            to_insert->parent = root;
        }
        else {
            ab_treenode_insert(root->right, to_insert, comparator);
        }
    }
}

void ab_tree_insert(ABTree* const tree, ABEntry* to_insert, ABComparatorType comparator)
{
    ABTreeNode* node = ab_treenode_new(to_insert);
    if(tree->root == NULL) {
        tree->root = node;
    }
    else {
        ab_treenode_insert(tree->root, node, comparator);
    }
}

static ABTreeNode* ab_treenode_delete(ABTreeNode* root, ABEntry* to_remove, ABComparatorType comparator, bool inside_job, bool *result)
{
    if(root == NULL)
        return root;
    if(comparator(root->entry, to_remove) < 0) {
        root->left = ab_treenode_delete(root->left, to_remove, comparator, inside_job, result);
    }
    else if(comparator(root->entry, to_remove) > 0) {
        root->right = ab_treenode_delete(root->right, to_remove, comparator, inside_job, result);
    }
    else if((!inside_job && comparator(root->entry, to_remove) == 0) || ab_compare_all(root->entry, to_remove) == 0) {
        if(root->left == NULL && root->right == NULL) {
            ab_entry_delete(root->entry);
            free(root);
            *result = true;
            return NULL;
        }
        else if(root->left == NULL) {
            ABTreeNode* tmp = root->right;
            tmp->parent = root->parent;
            ab_entry_delete(root->entry);
            free(root);
            *result = true;
            return tmp;
        }
        else if(root->right == NULL) {
            ABTreeNode* tmp = root->left;
            tmp->parent = root->parent;
            ab_entry_delete(root->entry);
            free(root);
            *result = true;
            return tmp;
        }
        else {
            ABTreeNode* tmp = ab_tree_min(root->right);
            root->entry = ab_entry_copy(tmp->entry);
            root->right = ab_treenode_delete(root->right, tmp->entry, comparator, true, result);
            *result = false;
        }
    }
    return root;
}

bool ab_tree_delete_first_match(ABTree* tree, ABEntry* to_remove, ABComparatorType comparator)
{
    bool result;
    tree->root = ab_treenode_delete(tree->root, to_remove, comparator, false, &result);
    return result;
}

void ab_tree_rebuild(ABTree* tree, ABComparatorType comparator)
{
    if(tree == NULL || tree->root == NULL) {
        return;
    }
    ABTreeNode* min_node = ab_tree_min(tree->root);
    ABTreeNode* max_node = ab_tree_max(tree->root);
    ABTreeNode* new_root = ab_treenode_new(ab_entry_copy(tree->root->entry));
    ABTreeNode* current = min_node;
    while(current != max_node) {
        ABEntry* entry_copy = ab_entry_copy(current->entry);
        ABTreeNode* new_node = ab_treenode_new(entry_copy);
        ab_treenode_insert(new_root, new_node, comparator);
        current = ab_tree_successor(current);
    }
    ab_treenode_delete_all(tree->root);
    tree->root = new_root;
}

ABEntry* ab_tree_search(ABTree* tree, ABEntry* equal_to, ABComparatorType comparator_tree, ABComparatorType comparator_value)
{
    ABTreeNode *current = tree->root;
    while(1) {
        if(current == NULL) {
            return NULL;
        }
        if(comparator_value(current->entry, equal_to )== 0) {
            return current->entry;
        }
        if(comparator_tree(current->entry, equal_to) < 0) {
            current = current->left;
        }
        if(comparator_tree(current->entry, equal_to) > 0) {
            current = current->right;
        }
    }
    return NULL;
}

ABList* ab_tree_find_all(ABTree* tree, ABEntry* equal_to, ABComparatorType comparator)
{
    ABList* found = ab_list_new();
    ABTreeNode* min_node = ab_tree_min(tree->root);
    ABTreeNode* max_node = ab_tree_max(tree->root);
    ABTreeNode* current = min_node;
    while(current != max_node) {
        if(comparator(current->entry, equal_to) == 0) {
            ab_list_insert_afterhead(found, current->entry);
        }
        current = ab_tree_successor(current);
    }
    return found;
}
