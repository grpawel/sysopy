#include "dir_tools.h"
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
mode_t mode_flags[] = {
[1] = S_IRUSR,
[2] = S_IWUSR,
[3] = S_IXUSR,
[4] = S_IRGRP,
[5] = S_IWGRP,
[6] = S_IXGRP,
[7] = S_IROTH,
[8] = S_IWOTH,
[9] = S_IXOTH
};

char mode_chars[] = {
[1] = 'r',
[2] = 'w',
[3] = 'x',
[4] = 'r',
[5] = 'w',
[6] = 'x',
[7] = 'r',
[8] = 'w',
[9] = 'x'
};

char* mode_to_str(mode_t mode)
{
    char* mode_str = malloc(sizeof(char) * 11);
    mode_str[0] = (S_ISDIR(mode)) ? 'd' : '-';
    for(int i=0; i < 10; i++) {
        if(mode & mode_flags[i]) {
            mode_str[i] = mode_chars[i];
        }
        else {
            mode_str[i] = '-';
        }
    }
    mode_str[10] = '\0';
    return mode_str;
}

char* path_add(const char* path_base, const char* name)
{
    size_t path_base_len = strlen(path_base);
    size_t name_len = strlen(name);
    size_t path_full_len = path_base_len + name_len + 2;
    char* path_full = malloc(sizeof(char) * path_full_len);
    strcpy(path_full, path_base);
    strcat(path_full, "/");
    strcat(path_full, name);
    return path_full;
}
