#ifndef _MISC_H
#define _MISC_H
char* strdup(const char* p);

int random_incl(int min, int max);

#define OUT
typedef int error_t;
typedef enum _Errors
{
    ERR_SUCCESS = 0,
    ERR_CANNOTOPENFILE,
    ERR_CANNOTACCESSFILE,
    ERR_CANNOTREADFILE,
    ERR_WRONGFILESIZE,
    ERR_CANNOTWRITEFILE,
    ERR_CANNOTCLOSEFILE,
    ERR_WRONG_OPERATION, //inside error
    ERR_WRONG_ARGUMENT
} Errors;

const char* error_to_string(error_t err);
#endif
