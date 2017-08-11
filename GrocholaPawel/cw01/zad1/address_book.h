//Pawel Grochola
#ifndef ADDRESS_BOOK_H
#define ADDRESS_BOOK_H
#include <time.h>
#include <stdbool.h>

typedef struct ABEntry
{
    char* name;
    char* surname;
    struct tm* birth_date;
    char* email;
    char* phone;
    char* address;
} ABEntry;

typedef enum ABField
{
    //ABF_ANY,
    ABF_NAME,
    ABF_SURNAME,
    ABF_BIRTHDATE,
    ABF_EMAIL,
    ABF_PHONE,
    ABF_ADDRESS
} ABField;

typedef enum ABStructure
{
    AB_LIST,
    AB_TREE
} ABStructure;

typedef struct ABAddressBookInternal ABAddressBookInternal;
typedef struct ABList ABList;
typedef struct ABAddressBook
{
    ABAddressBookInternal* internal;
    ABField ordered_by;
    void (*insert)(const void* self, ABEntry* to_insert);
    //deletes first matching entry in address book
    bool (*delete)(const void* self, ABEntry* to_delete);
    //in AddressBookEntry searches for matching entries; NULL values in to_find match everything
    ABList* (*find_all)(const void* self, char* name, char* surname, struct tm* birth_date, char* email, char* phone, char* address);
    ABList* (*find_all2)(const void* self, ABEntry* to_find);
    ABEntry* (*find_any)(const void* self, char* name, char* surname, struct tm* birth_date, char* email, char* phone, char* address);
    ABEntry* (*find_any2)(const void* self, ABEntry* to_find);
    void (*sort)(const void* self, ABField order_by);

} ABAddressBook;

ABAddressBook* ab_addressbook_new(ABStructure structureType, ABField order_by);
void ab_addressbook_delete(ABAddressBook** addressBook);

//Fields will be freed in delete_entry, so should be malloc'ed and unique among structures or NULL.
ABEntry* ab_entry_new(
    char* name,
    char* surname,
    struct tm* birth_date,
    char* email,
    char* phone,
    char* address);

ABEntry* ab_entry_copy(ABEntry* to_copy);
//Frees both entry and all associated fields.
void ab_entry_delete(ABEntry* entry);


void ab_entry_print(ABEntry* entry);
#endif
