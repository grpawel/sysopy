#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "locks.h"
#include "misc.h"
#include "options.h"
enum
{
OP_LOCK = 0,
OP_UNLOCK,
OP_WRITE,
OP_READ,
OP_BLOCK,
OP_NOBLOCK,
OP_LIST,
OP_QUIT,
OP_HELP
};
const int OP_COUNT = 9;
const char* OP_NAMES[] =
{
  [OP_LOCK] = "lock",
  [OP_UNLOCK] = "unlock",
  [OP_WRITE] = "write",
  [OP_READ] = "read",
  [OP_BLOCK] = "block",
  [OP_NOBLOCK] = "noblock",
  [OP_LIST] = "list",
  [OP_QUIT] = "quit",
  [OP_HELP] = "help"
};
static void print_help()
{
    printf("lock write/read <offset> [block] - lock byte at offset for writing or reading,");
    printf("                                   by default nonblocking\n");
    printf("unlock <offset>  - unlocks byte at offset\n");
    printf("list - list all locks by all processes\n");
    printf("read <offset> - reads byte from offset\n");
    printf("write <offset> <byte> - writes byte at offset\n");
    printf("quit - quits program\n");
    printf("help - prints this help\n");
}

int main(int argc, char* argv[])
{
    if(argc != 2) {
        printf("Wrong arguments - should be file name.\n");
        exit(1);
    }
    char* filename = argv[1];
    int pfd = open(filename, O_RDWR);
    if(pfd == -1) {
        printf("Cannot open file '%s'.\nReason: '%s'", filename, strerror(errno));
        exit(1);
    }
    char input;
    bool should_exit = false;
    do {
        char* opts = NULL;
        size_t len = 0;
        printf("> ");
        getline(&opts, &len, stdin);
        int words = count_words(opts);
        char** optv = split_to_words(opts);
        Options* options = options_parse(words, optv, OP_COUNT, OP_NAMES);
        if(options_contain(options, OP_LOCK)
                && options->not_matched_count == 1) {
            bool is_blocking = false;
            if(options_contain(options, OP_BLOCK)) {
                is_blocking = true;
            }
            int offset = atoi(options->not_matched[0]);
            int err = 0;
            if(options_contain(options, OP_WRITE)) {
                printf("Locking byte %d for writing...\n", offset);
                err = byte_setlock(pfd, offset, LOCK_WRITE, is_blocking);
            }
            else if(options_contain(options, OP_READ)) {
                printf("Locking byte %d for reading...\n", offset);
                err = byte_setlock(pfd, offset, LOCK_READ, is_blocking);
            }
            if(err) {
                printf("Cannot lock. Reason: %s\n", strerror(errno));
            }
            else {
                printf("Locked.\n");
            }
        }
        else if(options_contain(options, OP_UNLOCK)
                && options->not_matched_count == 1) {
            int offset = atoi(options->not_matched[0]);
            printf("Unlocked byte %d.\n", offset);
            byte_freelock(pfd, offset);
        }
        else if(options_contain(options, OP_LIST)) {
            printf("List of locks:\n");
            file_locks_traverse(pfd, lock_print, NULL);
            printf("\n");
        }
        else if(options_contain(options, OP_READ)
                && options->not_matched_count == 1) {
            int offset = atoi(options->not_matched[0]);
            Lock* lock;
            if((lock = byte_getlock(pfd, offset, LOCK_READ)) != NULL) {
                printf("Cannot read from %d. Reason: lock exists.\n", offset);
                free(lock);
            }
            else {
                uint8_t byte = byte_read(pfd, offset);
                printf("Read byte 0x%x ('%c') from %d.\n", byte, byte, offset);
            }
        }
        else if(options_contain(options, OP_WRITE)
                && options->not_matched_count == 2) {
            int offset = atoi(options->not_matched[0]);
            uint8_t byte = options->not_matched[1][0];
            Lock* lock;
            if((lock = byte_getlock(pfd, offset, LOCK_WRITE)) != NULL) {
                printf("Cannot write 0x%x ('%c') at %d. Reason: lock exists.\n", byte, byte, offset);
                free(lock);
            }
            else {
                printf("Writing byte 0x%x ('%c') at %d.\n", byte, byte, offset);
                byte_write(pfd, offset, byte);
            }
        }
        else if(options_contain(options, OP_QUIT)) {
            should_exit = true;
        }
        else if(options_contain(options, OP_HELP)) {
            print_help();
        }
        else {
            printf("Invalid command. Type 'help' for help.\n");
        }
        options_free(options);
        free(opts);
        for(int i=0; i < words; i++) {
            free(optv[i]);
        }
        free(optv);
    } while(!should_exit);
    close(pfd);
}

