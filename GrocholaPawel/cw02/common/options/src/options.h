#ifndef _OPTIONS_H
#define _OPTIONS_H
#include <stdint.h>
#include <stdbool.h>

typedef struct _Options
{
    uint32_t flags;
    //array with
    int not_matched_count;
    char** not_matched;

} Options;
//argc, argv - arguments from main
//optc - number of options, <= 32
//optv - array of option names
Options* options_parse(int argc, char** argv, int optc, const char* optv[]);

//option_id - index in array given to options_parse
//returns true if optv[option_id] was included in argv
bool options_contain(Options* options, uint32_t option_id);

void options_free(Options* options);
#endif
