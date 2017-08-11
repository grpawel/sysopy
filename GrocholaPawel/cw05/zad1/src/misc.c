#include "misc.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

char* strdup(const char* p)
{
    char* np = (char*)malloc(strlen(p)+1);
    return np ? strcpy(np, p) : np;
}

static bool is_whitespace(char c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

int count_words(char* str)
{
    int words = 0;
    if(str == NULL) return 0;
    bool is_word = !is_whitespace(str[0]);
    if(is_word) {
        ++words;
    }
    int str_len = strlen(str);
    for(int i = 1; i < str_len; i++) {
        if(is_whitespace(str[i])) {

            is_word = false;
        }
        else {
            if(!is_word) {
                ++words;
            }
            is_word = true;
        }
    }
    return words;
}

char** split_to_words(char* string)
{
    if(string == NULL || strlen(string) == 0) {
        return NULL;
    }
    int words = count_words(string);
    char** array = malloc(sizeof(char*) * words);
    char whitespace[] = " \t\n";
    char *word;
    char* string_cp = strdup(string);
    char* saveptr;
    word = strtok_r(string_cp, whitespace, &saveptr);
    int current = 0;
    while(word != NULL) {
        if(word == NULL) break;
        size_t len = strlen(word);
        array[current] = malloc(sizeof(char) * (len+1));
        memcpy(array[current], word, len+1);
        ++current;
        word = strtok_r(NULL, whitespace, &saveptr);
    }
    free(string_cp);
    return array;
}
