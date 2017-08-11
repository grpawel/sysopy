#include "dirsearch.h"
#ifdef DIRSTAT
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "dir_tools.h"

static void file_print(char* path, struct stat* filestat)
{
    intmax_t size = (intmax_t) filestat->st_size;
    char* permstr = mode_to_str(filestat->st_mode);
    char date[20];
    strftime(date, 20, "%F", localtime(&(filestat->st_mtime)));
    printf("%s\t%jd\t%s\t%s\n", path, size, permstr, date);
    free(permstr);
}

void dir_search(char* path, off_t maxsize)
{
    DIR* pdir = opendir(path);
    struct dirent* direntry;
    struct stat* fstat = malloc(sizeof(struct stat));
    while((direntry = readdir(pdir)) != NULL) {
        bool is_dotdir = strcmp(direntry->d_name, ".") == 0;
        bool is_dotdotdir = strcmp(direntry->d_name, "..") == 0;
        if(is_dotdir || is_dotdotdir) {
            continue;
        }
        char* path_cur = path_add(path, direntry->d_name);
        stat(path_cur, fstat);
        bool is_dir = S_ISDIR(fstat->st_mode);
        if(!is_dir) {
            if(fstat->st_size <= maxsize) {
                file_print(path_cur, fstat);
            }
        }
        else {
            dir_search(path_cur, maxsize);
        }
    }
}
#endif
#ifdef NFTW
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include "dir_tools.h"

static intmax_t max_size;

static int file_print(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ignored)
{
    if(typeflag != FTW_F) {
        return 0;
    }
    intmax_t size = (intmax_t) sb->st_size;
    if(size > max_size) {
        return 0;
    }
    char* permstr = mode_to_str(sb->st_mode);
    char date[20];
    strftime(date, 20, "%F", localtime(&(sb->st_mtime)));
    printf("%s\t%jd\t%s\t%s\n", fpath, size, permstr, date);
    free(permstr);
    return 0;
}
void dir_search(char* path, off_t maxsize)
{
    max_size = (intmax_t) maxsize;
    nftw(path, file_print, 64, FTW_PHYS);
}
#endif
