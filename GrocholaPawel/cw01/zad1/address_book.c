//Pawel Grochola
#include "address_book.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tree.h"
#include "tools.h"
#include "linked_list.h"


typedef struct ABAddressBookInternal
{
    void* contents;
    ABStructure structure_type;
} ABAddressBookInternal;

static void ab_addressbook_list_insert_entry(const void* self, ABEntry* to_insert);
static bool ab_addressbook_list_delete_entry(const void* self, ABEntry* to_delete);
static ABList* ab_addressbook_list_find_all_entries(const void* self,
        char* name,char* surname, struct tm* birth_date,char* email,char* phone,char* address);
static ABList* ab_addressbook_list_find_all_entries2(const void* self, ABEntry* to_find);
static ABEntry* ab_addressbook_list_find_any(const void* self,
        char* name, char* surname, struct tm* birth_date, char* email, char* phone, char* address);
static ABEntry* ab_addressbook_list_find_any2(const void* self, ABEntry* to_find);
static void ab_addressbook_list_sort(const void* self, ABField order_by);

static void ab_addressbook_tree_insert_entry(const void* self, ABEntry* to_insert);
static bool ab_addressbook_tree_delete_entry(const void* self, ABEntry* to_delete);
static ABList* ab_addressbook_tree_find_all_entries(const void* self,
        char* name, char* surname, struct tm* birth_date, char* email, char* phone, char* address);
static ABList* ab_addressbook_tree_find_all_entries2(const void* self, ABEntry* to_find);
static ABEntry* ab_addressbook_tree_find_any(const void* self,
        char* name, char* surname, struct tm* birth_date, char* email, char* phone, char* address);
static ABEntry* ab_addressbook_tree_find_any2(const void* self, ABEntry* to_find);
static void ab_addressbook_tree_rebuild(const void* self, ABField order_by);

ABAddressBook* ab_addressbook_new(ABStructure structure_type, ABField order_by)
{
    ABAddressBook* book = malloc(sizeof(ABAddressBook));
    book->internal = malloc(sizeof(ABAddressBookInternal));
    book->ordered_by = order_by;
    book->internal->structure_type = structure_type;
    switch(structure_type) {
        case AB_LIST: {
            ABList* list = ab_list_new();
            book->internal->contents = list;
            book->insert = ab_addressbook_list_insert_entry;
            book->delete = ab_addressbook_list_delete_entry;
            book->find_all = ab_addressbook_list_find_all_entries;
            book->find_all2 = ab_addressbook_list_find_all_entries2;
            book->find_any = ab_addressbook_list_find_any;
            book->find_any2 = ab_addressbook_list_find_any2;
            book->sort = ab_addressbook_list_sort;
            break;
        }
        case AB_TREE: {
            book->internal->contents = ab_tree_new();
            book->insert = ab_addressbook_tree_insert_entry;
            book->delete = ab_addressbook_tree_delete_entry;
            book->find_all = ab_addressbook_tree_find_all_entries;
            book->find_all2 = ab_addressbook_tree_find_all_entries2;
            book->find_any = ab_addressbook_tree_find_any;
            book->find_any2 = ab_addressbook_tree_find_any2;
            book->sort = ab_addressbook_tree_rebuild;
            break;
        }
    }
    return book;
}

void ab_addressbook_delete(ABAddressBook** addressBook)
{
    if(*addressBook == NULL) return;
    switch((*addressBook)->internal->structure_type) {
        case AB_LIST: {
            ab_list_delete((ABList*) (*addressBook)->internal->contents);
            break;
        }
        case AB_TREE: {
            ab_tree_delete((ABTree*) (*addressBook)->internal->contents);
            break;
        }
    }
    free((*addressBook)->internal);
    free(*addressBook);
    *addressBook = NULL;
}

//Fields will be copied in result structure to allow freeing them
ABEntry* ab_entry_new(
    char* name,
    char* surname,
    struct tm* birth_date,
    char* email,
    char* phone,
    char* address)
{
    ABEntry* entry = malloc(sizeof(ABEntry));
    entry->name = name != NULL ? strdup(name) : NULL;
    entry->surname = surname != NULL ? strdup(surname) : NULL;
    struct tm* birthdate_copy;
    if(birth_date != NULL) {
        birthdate_copy = malloc(sizeof(struct tm));
        memcpy(birthdate_copy, birth_date, sizeof(struct tm));
    }
    else {
        birthdate_copy = NULL;
    }
    entry->birth_date = birthdate_copy;
    entry->email = email != NULL ? strdup(email) : NULL;
    entry->phone = phone != NULL ? strdup(phone) : NULL;
    entry->address = address != NULL ? strdup(address) : NULL;
    return entry;
}

//Frees both entry and all associated fields.
void ab_entry_delete(ABEntry* entry)
{
    free(entry->name);
    free(entry->surname);
    free(entry->birth_date);
    free(entry->email);
    free(entry->phone);
    free(entry->address);
    free(entry);
}
/*
static char* strdup(const char* p)
{
    char* np = (char*)malloc(strlen(p)+1);
    return np ? strcpy(np, p) : np;
}
*/

ABEntry* ab_entry_copy(ABEntry* to_copy)
{
    return ab_entry_new(
        to_copy->name,
        to_copy->surname,
        to_copy->birth_date,
        to_copy->email,
        to_copy->phone,
        to_copy->address);
}

static void ab_addressbook_list_insert_entry(const void* self, ABEntry* to_insert)
{
    ABListNode* node = ab_listnode_new(to_insert);
    ABAddressBook* book = (ABAddressBook*) self;
    ABList* list = (ABList*) book->internal->contents;
    ABComparatorType comparator = ab_comparator_get(book->ordered_by);
    ab_list_insert_sorted(list, node, comparator);
}

static bool ab_addressbook_list_delete_entry(const void* self, ABEntry* to_delete)
{
    ABAddressBook* book = (ABAddressBook*) self;
    ABList* list = (ABList*) book->internal->contents;
    return ab_list_delete_first_match(list, to_delete, ab_compare_all_specialnulls);
}

static ABList* ab_addressbook_list_find_all_entries(const void* self,
        char* name, char* surname, struct tm* birth_date, char* email, char* phone, char* address)
{
    ABEntry* to_find = ab_entry_new(name, surname, birth_date, email, phone, address);
    ABAddressBook* book = (ABAddressBook*) self;
    ABList* list = (ABList*) book->internal->contents;
    ABList* found = ab_list_copy_all_equal(list, to_find, ab_compare_all_specialnulls);
    ab_entry_delete(to_find);
    return found;
}

static ABList* ab_addressbook_list_find_all_entries2(const void* self, ABEntry* to_find)
{
    ABAddressBook* book = (ABAddressBook*) self;
    ABList* list = (ABList*) book->internal->contents;
    return ab_list_copy_all_equal(list, to_find, ab_compare_all_specialnulls);
}
static ABEntry* ab_addressbook_list_find_any(const void* self,
        char* name, char* surname, struct tm* birth_date, char* email, char* phone, char* address)
{
    ABEntry* to_find = ab_entry_new(name, surname, birth_date, email, phone, address);
    ABAddressBook* book = (ABAddressBook*) self;
    ABList* list = (ABList*) book->internal->contents;
    ABEntry* found = ab_list_find_any(list, to_find, ab_compare_all_specialnulls);
    return found;
}

static ABEntry* ab_addressbook_list_find_any2(const void* self, ABEntry* to_find)
{
    ABAddressBook* book = (ABAddressBook*) self;
    ABList* list = (ABList*) book->internal->contents;
    ABEntry* found = ab_list_find_any(list, to_find, ab_compare_all_specialnulls);
    return found;
}

static void ab_addressbook_list_sort(const void* self, ABField order_by)
{
    ABAddressBook* book = (ABAddressBook*) self;
    ABList* list = (ABList*) book->internal->contents;
    ab_list_sort(list, ab_comparator_get(order_by));
    book->ordered_by = order_by;
}

static void ab_addressbook_tree_insert_entry(const void* self, ABEntry* to_insert)
{
    ABAddressBook* book = (ABAddressBook*) self;
    ABTree* tree = (ABTree*) book->internal->contents;
    ABComparatorType comparator = ab_comparator_get(book->ordered_by);
    ab_tree_insert(tree, to_insert, comparator);
}

static bool ab_addressbook_tree_delete_entry(const void* self, ABEntry* to_delete)
{
    ABAddressBook* book = (ABAddressBook*) self;
    ABTree* tree = (ABTree*) book->internal->contents;
    return ab_tree_delete_first_match(tree, to_delete, ab_compare_all_specialnulls);
}

static ABList* ab_addressbook_tree_find_all_entries(const void* self,
        char* name, char* surname, struct tm* birth_date, char* email, char* phone, char* address)
{
    ABAddressBook* book = (ABAddressBook*) self;
    ABTree* tree = (ABTree*) book->internal->contents;
    ABEntry* to_find = ab_entry_new(name, surname, birth_date,email,phone,address);
    ABList* found = ab_tree_find_all(tree, to_find, ab_compare_all_specialnulls);
    ab_entry_delete(to_find);
    return found;
}

static ABList* ab_addressbook_tree_find_all_entries2(const void* self, ABEntry* to_find)
{
    ABAddressBook* book = (ABAddressBook*) self;
    ABTree* tree = (ABTree*) book->internal->contents;
    ABList* found = ab_tree_find_all(tree, to_find, ab_compare_all_specialnulls);
    return found;
}

static ABEntry* ab_addressbook_tree_find_any(const void* self,
    char* name, char* surname, struct tm* birth_date, char* email, char* phone, char* address)
{
    ABAddressBook* book = (ABAddressBook*) self;
    ABTree* tree = (ABTree*) book->internal->contents;
    ABEntry* to_find = ab_entry_new(name, surname, birth_date,email,phone,address);
    ABEntry* found = ab_tree_search(tree, to_find, ab_comparator_get(book->ordered_by), ab_compare_all_specialnulls);
    ab_entry_delete(to_find);
    return found;
}
static ABEntry* ab_addressbook_tree_find_any2(const void* self, ABEntry* to_find)
{
    ABAddressBook* book = (ABAddressBook*) self;
    ABTree* tree = (ABTree*) book->internal->contents;
    ABEntry* found = ab_tree_search(tree, to_find, ab_comparator_get(book->ordered_by), ab_compare_all_specialnulls);
    return found;
}

static void ab_addressbook_tree_rebuild(const void* self, ABField order_by)
{
    ABAddressBook* book = (ABAddressBook*) self;
    ABTree* tree = (ABTree*) book->internal->contents;
    ab_tree_rebuild(tree, ab_comparator_get(order_by));
    book->ordered_by = order_by;
}

void ab_entry_print(ABEntry* entry)
{
    char birthdate[80];
    if(entry->birth_date != NULL) {
        strftime(birthdate, 80, "%F", entry->birth_date);
    }
    else {
        strcpy(birthdate, "NULL");
    }
    printf("nm: '%s' srnm: '%s' bd: '%s' mail: '%s' tel: '%s' adrs: '%s'\n", entry->name, entry->surname,
    birthdate, entry->email, entry->phone, entry->address);
}
