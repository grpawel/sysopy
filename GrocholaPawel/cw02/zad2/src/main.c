#include <stdio.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "dirsearch.h"

int main(int argc, char* argv[])
{
    if(argc != 3) {
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }
    char* path = argv[1];
    int size = atoi(argv[2]);
    char path_real[PATH_MAX];
    realpath(path, path_real);
    dir_search(path_real, (off_t) size);
    return 0;
}



