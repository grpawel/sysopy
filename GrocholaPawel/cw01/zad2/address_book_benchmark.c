#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "../zad1/address_book.h"

typedef void* (*Operation)(void*);

struct timeval timeval_diff(struct timeval time0, struct timeval time1)
{
    struct timeval diff = {time1.tv_sec - time0.tv_sec,
                                time1.tv_usec - time0.tv_usec};
    if (diff.tv_usec < 0) {
        diff.tv_usec += 1000000;
        diff.tv_sec -= 1;
    }
    return diff;
}

void* measure_print_time(Operation operation, void* args, const char* op_name)
{
    struct timeval real0;
    struct timeval real1;
    struct rusage* usage = malloc(sizeof(struct rusage));
    struct timeval user0, user1;
    struct timeval system0, system1;
    gettimeofday(&real0, NULL);
    getrusage(RUSAGE_SELF, usage);
    user0 = usage->ru_utime;
    system0 = usage->ru_stime;

    void* result = operation(args);

    gettimeofday(&real1, NULL);
    getrusage(RUSAGE_SELF, usage);
    user1 = usage->ru_utime;
    system1 = usage->ru_stime;
    struct timeval realdiff = timeval_diff(real0, real1);
    struct timeval userdiff = timeval_diff(user0, user1);
    struct timeval sysdiff = timeval_diff(system0, system1);
    printf("%s:\n", op_name);
    printf("\tReal: %ld.%06ld\t User:%ld.%06ld\t Sys: %ld.%06ld\n",
        realdiff.tv_sec, realdiff.tv_usec,
        userdiff.tv_sec, userdiff.tv_usec,
        sysdiff.tv_sec, sysdiff.tv_usec);
    free(usage);
    return result;
}

char* alloc_random_string(int len)
{
    char *str = malloc(len+1);
    for(int i=0; i < len; i++) {
        str[i] = rand() % ('z'-'a'+1) + 'a';
    }
    str[len] = '\0';
    return str;
}
ABEntry* create_random_entry(int id)
{
    const int RANDOM_STRING_LEN = 6;
    const int FIELD_LEN = 24;
    char name[FIELD_LEN];
    char surname[FIELD_LEN];
    struct tm* birthdate = malloc(sizeof(struct tm));
    birthdate->tm_year = rand() % 50 + 1950;
    birthdate->tm_mon = rand() % 12;
    birthdate->tm_mday = rand() % 28;
    mktime(birthdate);
    char email[FIELD_LEN];
    char phone[FIELD_LEN];
    char address[FIELD_LEN];

    char* rand_str = alloc_random_string(RANDOM_STRING_LEN);
    sprintf(name, "NAME%s #%d", rand_str, id);
    sprintf(surname, "SURNAME%s #%d", rand_str, id);
    free(rand_str);
    rand_str = alloc_random_string(RANDOM_STRING_LEN);
    sprintf(email, "EMAIL%s #%d", rand_str, id);
    sprintf(phone, "PHONE%s #%d", rand_str, id);
    sprintf(address, "ADDRESS%s #%d", rand_str, id);
    free(rand_str);
    free(birthdate);
    return ab_entry_new(name, surname, birthdate, email, phone, address);
}
struct EntryData {
    ABEntry** entries;
    ABAddressBook* book;
    int count;
};

void* op_create_list(void* ignored)
{
    ABAddressBook* book = ab_addressbook_new(AB_LIST, ABF_SURNAME);
    return book;
}

void* op_create_tree(void* ignored)
{
    ABAddressBook* book = ab_addressbook_new(AB_TREE, ABF_SURNAME);
    return book;
}

void* op_fill_n_entries(void* d)
{
    struct EntryData* data = (struct EntryData*) d;
    for(int i=0; i < data->count; i++) {
        data->book->insert(data->book, data->entries[i]);
    }
    return NULL;
}

void* op_sort_rebuild(void* b)
{
    ABAddressBook* book = (ABAddressBook*) b;
    book->sort(book, ABF_EMAIL);
    return NULL;
}

void* op_find_any(void* d)
{
    struct EntryData* data = (struct EntryData*) d;
    data->book->find_any2(data->book, data->entries[0]);
    return NULL;
}

void* op_find_all(void* d)
{
    struct EntryData* data = (struct EntryData*) d;
    data->book->find_all2(data->book, data->entries[0]);
    return NULL;
}

void* op_delete_book(void* b)
{
    ABAddressBook* book = (ABAddressBook*) b;
    ab_addressbook_delete(&book);
    return NULL;
}

void* op_delete_entry(void* d)
{
    struct EntryData* data = (struct EntryData*) d;
    data->book->delete(data->book, data->entries[0]);
    return NULL;
}
void test_address_book_times(ABAddressBook* book)
{
    const int ENTRY_COUNT = 1000;
    struct EntryData data;
    data.book = book;
    data.count = 1;
    data.entries = malloc(sizeof(ABEntry*));
    data.entries[0] = create_random_entry(0);
    measure_print_time(op_fill_n_entries, &data, "Inserting first entry to empty book");

    struct EntryData data2;
    data2.book = book;
    data2.count = ENTRY_COUNT;
    data2.entries = malloc(sizeof(ABEntry*)*ENTRY_COUNT);
    for(int i=0; i < ENTRY_COUNT; i++) {
        data2.entries[i] = create_random_entry(i+1);
    }
    measure_print_time(op_fill_n_entries, &data2, "Inserting next 1000 entries");

    struct EntryData data3;
    data3.book = book;
    data3.count = 1;
    data3.entries = malloc(sizeof(ABEntry*));
    data3.entries[0] = create_random_entry(ENTRY_COUNT+2);
    measure_print_time(op_fill_n_entries, &data3, "Inserting #1002 entry");
    measure_print_time(op_find_any, &data, "Finding first matching entry");
    measure_print_time(op_find_all, &data, "Finding all matching entries");
    measure_print_time(op_find_all, NULL, "Deleting entry");
    measure_print_time(op_sort_rebuild, book, "Sorting list by email");
    measure_print_time(op_delete_book, book, "Deleting book");
    free(data.entries);
    free(data2.entries);
    free(data3.entries);
}

int main()
{
    ABAddressBook* book;
    book = (ABAddressBook*) measure_print_time(op_create_list, NULL, "Creating address book using list");
    test_address_book_times(book);
    printf("\n");
    book = (ABAddressBook*) measure_print_time(op_create_tree, NULL, "Creating address book using tree");
    test_address_book_times(book);

}

