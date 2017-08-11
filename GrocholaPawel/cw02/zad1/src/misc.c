#include "misc.h"
#include <stdlib.h>
#include <string.h>
char* strdup(const char* p)
{
    char* np = (char*)malloc(strlen(p)+1);
    return np ? strcpy(np, p) : np;
}
int random_incl(int min, int max)
{
    return rand() % (max - min + 1) + min;
}
const char* error_to_string(error_t err)
{
    switch(err) {
        case ERR_SUCCESS: return "No error happened - this should not be printed.";
        case ERR_CANNOTOPENFILE: return "Cannot open file.";
        case ERR_CANNOTACCESSFILE: return "Cannot access file.";
        case ERR_CANNOTREADFILE: return "Cannot read file.";
        case ERR_WRONGFILESIZE: return "File has different size than specified.";
        case ERR_CANNOTWRITEFILE: return "Cannot write to file.";
        case ERR_CANNOTCLOSEFILE: return "Cannot close file.";
        case ERR_WRONG_OPERATION: return "Program tried to perform invalid operation on file.";
        case ERR_WRONG_ARGUMENT: return "Wrong argument in command line.";
        default: return "Unknown error.";
    }
}
