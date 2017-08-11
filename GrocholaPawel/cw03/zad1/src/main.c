#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "script.h"
#include "errors.h"
#ifdef ZADANIE_2
#include <sys/time.h>
#include <sys/resource.h>
#endif
err_t script_open_run(char* filename)
{
    FILE* file = fopen(filename, "r");
    if(file == NULL) {
        return ERR_CANNOT_OPEN_FILE;
    }
    run_script(file);
    fclose(file);
    return ERR_SUCCESS;
}
#ifdef ZADANIE_2
void print_rusage()
{
    struct rusage ru;
    getrusage(RUSAGE_CHILDREN, &ru);
    printf("User time:   %lld s\n", (long long) ru.ru_utime.tv_sec);
    printf("System time: %lld s\n", (long long) ru.ru_stime.tv_sec);
}
#endif
int main(int argc, char* argv[])
{
#ifdef ZADANIE_2
    if(argc != 4) {
        fprintf(stderr, "Bad arguments. Required three argument - path to file with batch script, CPU limit and address space limit.\n");
        exit(1);
    }
    cpu_limit = atoi(argv[2]);
    as_limit = atoi(argv[3]);
#else
    if(argc != 2) {
        fprintf(stderr, "Bad arguments. Required single argument - path to file with batch script");
        exit(1);
    }
#endif
    char* filename = argv[1];
    err_t err = script_open_run(filename);
    if(err != ERR_SUCCESS) {
        fprintf(stderr, "%s\n", err_to_string(err));
        if(err_is_system(err)) {
            fprintf(stderr, "%s\n", strerror(errno));
        }
    }
#ifdef ZADANIE_2
    print_rusage();
#endif
    return 0;
}
