#include "options.h"
#include <string.h>
#include <stdlib.h>


static char* strdup(const char* p)
{
    char* np = (char*)malloc(strlen(p)+1);
    return np ? strcpy(np, p) : np;
}

bool options_contain(Options* options, uint32_t option_id)
{
    return options->flags & (1 << option_id);
}

Options* options_parse(int argc, char** argv, int optc, const char* optv[])
{
    Options* options = malloc(sizeof(Options));
    options->flags = 0u;
    uint32_t not_matched_indexes = 0u;
    int not_matched_count = 0;

    int arg_cur = 0;
    while(arg_cur < argc) {
        bool matched = false;
        int opt_cur = 0;
        while(opt_cur < optc && !matched) {
            if(strcmp(argv[arg_cur], optv[opt_cur]) == 0) {
                matched = true;
                options->flags |= (1 << opt_cur);
            }
            ++opt_cur;
        }
        if(!matched) {
            ++not_matched_count;
            not_matched_indexes |= (1 << arg_cur);
        }
        ++arg_cur;
    }
    options->not_matched_count = not_matched_count;
    options->not_matched = malloc(sizeof(char*) * not_matched_count);
    int not_matched_index = 0;
    for(int i = 0; i < argc; ++i) {
        if(not_matched_indexes & (1<<i)) {
            options->not_matched[not_matched_index++] = strdup(argv[i]);
        }
    }
    return options;
}

void options_free(Options* options)
{
    for(int i=0; i < options->not_matched_count; ++i) {
        free(options->not_matched[i]);
    }
    free(options->not_matched);
    free(options);
}
