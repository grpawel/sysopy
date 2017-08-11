#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "logger.h"
#include "utils.h"
#include "threads.h"

int* numbers;
int numbers_size = 10;
int number_min = 0;
int number_max = 1000;
pthread_t* writers;
int writers_count = 1;
pthread_t* readers;
int readers_count = 1;

int reads_to_do = 1;
int writes_to_do = 1;

pthread_mutex_t threads_creating_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t threads_creating_cond = PTHREAD_COND_INITIALIZER;
bool threads_created = false;

void reader_read(int divider)
{
    int matches = 0;
    for(int i = 0; i < numbers_size; i++) {
        if(numbers[i] % divider == 0) {
            matches++;
            log_detail("Found match for divider %d: %d (pos %d)", divider, numbers[i], i);
        }
    }
    log_info("Found %d matches for divider %d.", matches, divider);
}

void writer_write()
{
    int to_write = random_incl(0, numbers_size);
    bool indices_to_write[numbers_size];
    for(int i = 0; i < numbers_size; i++) {
        indices_to_write[i] = false;
    }
    int chosen_indices = 0;
    while(chosen_indices < to_write) {
        int try_index = random_incl(0, numbers_size);
        if(!indices_to_write[try_index]) {
            indices_to_write[try_index] = true;
            chosen_indices++;
        }
    }
    for(int i = 0; i < numbers_size; i++) {
        if(indices_to_write[i]) {
            numbers[i] = random_incl(number_min, number_max);
            log_detail("Writing %d at pos %d.", numbers[i], i);
        }
    }
    log_info("Written %d numbers.", to_write);
}


void print_help(void) {
    printf("Options: \n");
    printf("-r10 - 10 readers (default 1)\n");
    printf("-w10 - 10 writers (default 1)\n");
    printf("-s10 - size of array is 10 (default 10)\n");
    printf("-n10 - reader makes 10 reads then exits (default 1)\n");
    printf("-m10 - writer makes 10 writes then exits (default 1)\n");
    printf("-i   - print detailed info about read and write operations\n");
    printf("-d   - print debug info\n");
    printf("-h   - print this help\n");
    printf("-0   - do not print date and time\n");
}

void header_none(enum log_level level, FILE* stream) {
    (void) level;
    (void) stream;
}

int main(int argc, char* argv[])
{
    log_init();
    srand(time(NULL));
    for(int i=1; i < argc; i++) {
        int number = argv[i][0] != '\0' && argv[i][1] != '\0' ? atoi(argv[i]+2) : 0;
        switch(argv[i][1]) {
            case 'r': {
                readers_count = number;
                break;
            }
            case 'w': {
                writers_count = number;
                break;
            }
            case 's': {
                numbers_size = number;
                break;
            }
            case 'n': {
                reads_to_do = number;
                break;
            }
            case 'm': {
                writes_to_do = number;
                break;
            }
            case 'i': {
                log_enable(LOG_DETAIL);
                break;
            }
            case 'd': {
                log_enable(LOG_DEBUG);
                break;
            }
            case 'h': {
                print_help();
                break;
            }
            case '1': {
                log_set_header(header_none);
                break;
            }
        }
    }
    //allocating memory
    if(!(numbers = malloc(sizeof(int) * numbers_size))) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if(!(readers = malloc(sizeof(pthread_t) * readers_count))) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if(!(writers = malloc(sizeof(pthread_t) * writers_count))) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    //fill numbers array
    for(int i = 0; i < numbers_size; i++) {
        numbers[i] = random_incl(number_min, number_max);
    }
    //creating threads
    init_threads();

    pthread_mutex_lock(&threads_creating_mutex);
    int* reader_args = malloc(sizeof(int) * readers_count);
    for(int i = 0; i < readers_count; i++) {
        int ret;
        log_debug("Creating reader %d of %d.", i + 1, readers_count);
        reader_args[i] = i + 2;
        if((ret = pthread_create(readers+i, NULL, reader_thread, reader_args + i))) {
            log_error("Cannot create reader thread: %d %s", ret, strerror(ret));
            readers[i] = 0;
        }
    }

    for(int i = 0; i < writers_count; i++) {
        int ret;
        log_debug("Creating writer %d of %d.", i + 1, writers_count);
        if((ret = pthread_create(writers+i, NULL, writer_thread, NULL))) {
            log_error("Cannot create writer thread: %d %s", ret, strerror(ret));
            writers[i] = 0;
        }
    }
    threads_created = true;
    pthread_cond_broadcast(&threads_creating_cond);
    pthread_mutex_unlock(&threads_creating_mutex);

    //joining threads
    for(int i = 0; i < writers_count; i++) {
        int ret;
        if((ret = pthread_join(writers[i], NULL))) {
            log_error("Cannot join writer thread: %d %s", ret, strerror(ret));
        }
    }
    for(int i = 0; i < readers_count; i++) {
        int ret;
        if((ret = pthread_join(readers[i], NULL))) {
            log_error("Cannot join reader thread: %d %s", ret, strerror(ret));
        }
    }
    deinit_threads();
    //cleanup:
    free(reader_args);
    free(numbers);
    free(readers);
    free(writers);

    return 0;
}
