//Pawel Grochola
#include "comparators.h"
#include <string.h>
#include <time.h>

static int ab_compare_name(ABEntry* a, ABEntry* b);
static int ab_compare_surname(ABEntry* a, ABEntry* b);
static int ab_compare_birthdate(ABEntry* a, ABEntry* b);
static int ab_compare_email(ABEntry* a, ABEntry* b);
static int ab_compare_phone(ABEntry* a, ABEntry* b);
static int ab_compare_address(ABEntry* a, ABEntry* b);

ABComparatorType ab_comparator_get(ABField field_to_compare)
{
    switch(field_to_compare) {
        case ABF_NAME:
            return ab_compare_name;
        case ABF_SURNAME:
            return ab_compare_surname;
        case ABF_BIRTHDATE:
            return ab_compare_birthdate;
        case ABF_EMAIL:
            return ab_compare_email;
        case ABF_PHONE:
            return ab_compare_phone;
        case ABF_ADDRESS:
            return ab_compare_address;
        default:
            return NULL;
    }
}

//order of fields to be compared: name,surname,birth_date,email,phone,address
//NULL in bmatches anything
/*
int ab_compare_all(ABEntry* a, ABEntry* b)
{
    int result = ab_compare_name(a, b);
    if(result != 0 && b->name != NULL) return result;
    result = ab_compare_surname(a,b);
    if(result != 0 && b->surname != NULL) return result;
    result = ab_compare_email(a,b);
    if(result != 0 && b->email != NULL) return result;
    result = ab_compare_phone(a,b);
    if(result != 0 && b->phone != NULL) return result;
    result = ab_compare_address(a,b);
    if(result != 0 && b->address != NULL) return result;
    result = ab_compare_birthdate(a,b);
    if(result != 0 && b->birth_date != NULL) return result;
    return 0;

}
*/
int ab_compare_all_specialnulls(ABEntry* a, ABEntry* b)
{
    int result = ab_compare_name(a, b);
    if(result != 0 && b->name != NULL)
        return result;
    result = ab_compare_surname(a,b);
    if(result != 0 && b->surname != NULL)
        return result;
    result = ab_compare_email(a,b);
    if(result != 0 && b->email != NULL)
        return result;
    result = ab_compare_phone(a,b);
    if(result != 0 && b->phone != NULL)
        return result;
    result = ab_compare_address(a,b);
    if(result != 0 && b->address != NULL)
        return result;
    result = ab_compare_birthdate(a,b);
    if(result != 0 && b->birth_date != NULL)
        return result;
    return 0;
}

int ab_compare_all(ABEntry* a, ABEntry* b)
{
    int result = ab_compare_name(a, b);
    if(result != 0)
        return result;
    result = ab_compare_surname(a,b);
    if(result != 0)
        return result;
    result = ab_compare_email(a,b);
    if(result != 0)
        return result;
    result = ab_compare_phone(a,b);
    if(result != 0)
        return result;
    result = ab_compare_address(a,b);
    if(result != 0)
        return result;
    result = ab_compare_birthdate(a,b);
    if(result != 0)
        return result;
    return 0;
}

static int ab_compare_name(ABEntry* a, ABEntry* b)
{
    if(a->name == NULL && b->name == NULL)
        return 0;
    if(a->name == NULL)
        return -1;
    if(b->name == NULL)
        return 1;
    return strcmp(a->name,b->name);
}

static int ab_compare_surname(ABEntry* a, ABEntry* b)
{
    if(a->surname == NULL && b->surname == NULL)
        return 0;
    if(a->surname == NULL)
        return -1;
    if(b->surname == NULL)
        return 1;
    return strcmp(a->surname,b->surname);
}

static int ab_compare_birthdate(ABEntry* a, ABEntry* b)
{
    if(a->birth_date == NULL && b->birth_date == NULL)
        return 0;
    if(a->birth_date == NULL)
        return -1;
    if(b->birth_date == NULL)
        return 1;
    time_t a_birthtime = mktime((struct tm*) a->birth_date);
    time_t b_birthtime = mktime((struct tm*) b->birth_date);
    return difftime(a_birthtime, b_birthtime);
}

static int ab_compare_email(ABEntry* a, ABEntry* b)
{
    if(a->email == NULL && b->email == NULL)
        return 0;
    if(a->email == NULL)
        return -1;
    if(b->email == NULL)
        return 1;
    return strcmp(a->email,b->email);
}

static int ab_compare_phone(ABEntry* a, ABEntry* b)
{
    if(a->phone == NULL && b->phone == NULL)
        return 0;
    if(a->phone == NULL)
        return -1;
    if(b->phone == NULL)
        return 1;
    return strcmp(a->phone,b->phone);
}

static int ab_compare_address(ABEntry* a, ABEntry* b)
{
    if(a->address == NULL && b->address == NULL)
        return 0;
    if(a->address == NULL)
        return -1;
    if(b->address == NULL)
        return 1;
    return strcmp(a->address,b->address);
}
