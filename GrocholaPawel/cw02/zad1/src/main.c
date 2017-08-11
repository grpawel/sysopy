#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include "options.h"
#include "filefunc.h"
#include "file_manip.h"
#include "misc.h"
enum
{
AR_GEN = 0,
AR_SHUF,
AR_SORT,
AR_SYS,
AR_LIB
};
const char *ARG_NAMES[] =
{
  [AR_GEN] = "generate",
  [AR_SHUF] = "shuffle",
  [AR_SORT] = "sort",
  [AR_SYS] = "sys",
  [AR_LIB] = "lib"
};
void test(void);
int main(int argc, char* argv[])
{
    srand(time(NULL));
    FunctionSet fs;
    int record_count;
    ssize_t record_size;
    FileOper* foper;
    Options* options = options_parse(argc, argv, 5, ARG_NAMES);
    if(options->not_matched_count < 3) {
        printf("Please specify file name, record number and record size.\n");
        exit(1);
    }
    record_count = atoi(options->not_matched[2]);
    if(record_count <= 0) {
        printf("Invalid number of records: '%s'\n", options->not_matched[2]);
        exit(1);
    }
    record_size = (ssize_t) atoi(options->not_matched[3]);
    if(record_size <= 0) {
        printf("Invalid size of records: '%s'\n", options->not_matched[3]);
        exit(1);
    }
    if(options_contain(options, AR_LIB)) {
        fs = FS_LIB;
    }
    else if(options_contain(options, AR_SYS)) {
        fs = FS_SYS;
    }
    else {
        printf("Please specify set of functions to use.\n");
        exit(1);
    }

    error_t err = ERR_SUCCESS;
    error_t err2 = ERR_SUCCESS;
    if(options_contain(options, AR_SHUF)) {
        err = foper_open(&foper, fs, record_size, record_count, options->not_matched[1], FILE_READ | FILE_WRITE);
        err2 = file_shuffle(foper);
    }
    else if(options_contain(options, AR_SORT)) {
        err = foper_open(&foper, fs, record_size, record_count, options->not_matched[1], FILE_READ | FILE_WRITE);
        err2 = file_bubblesort(foper);
    }
    else if(options_contain(options, AR_GEN)) {
        err = foper_open(&foper, fs, record_size, record_count, options->not_matched[1], FILE_WRITE | FILE_CREATE);
        err2 = file_generate(foper);
    }
    else {
        printf("Please add command for file.\n");
        foper->close(&foper);
        exit(1);
    }
    if(err != ERR_SUCCESS || err2 != ERR_SUCCESS) {
        printf("Error: '%s'. Opened file: '%s'\n", error_to_string(err ? err : err2), options->not_matched[1]);
    }
    foper->close(&foper);
    options_free(options);
    return 0;

}
