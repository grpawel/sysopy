//Pawel Grochola
#ifndef COMPARATORS_H
#define COMPARATORS_H
#include "address_book.h"

typedef int (*ABComparatorType)(ABEntry*, ABEntry*);

//returns function to compare entries on single field
ABComparatorType ab_comparator_get(ABField field_to_compare);

//function to compare entries on all fields
//returns 0 if all fields are equal
//order of fields to be compared: name,surname,birth_date,email,phone,address
//NULL in b matches anything
int ab_compare_all_specialnulls(ABEntry* a, ABEntry* b);
int ab_compare_all(ABEntry* a, ABEntry* b);
#endif
