#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#define RECORD_SIZE 1024
#define RECORD_TEXT_LEN ((RECORD_SIZE) - sizeof(int))
int threads_number = 0;
pthread_t* threads = NULL;
char* file_name = NULL;
int records_to_read = 0;
char* searched = NULL;
pthread_mutex_t file_lock;
int fd = -1;
bool file_ended = false;
pthread_mutex_t thread_control;

struct record {
    int id;
    char text[RECORD_TEXT_LEN];
};




void* read_thread(void* arg)
{
    if(pthread_mutex_lock(&thread_control) == EOWNERDEAD) {
        pthread_mutex_consistent(&thread_control);
    }
    pthread_mutex_unlock(&thread_control);
    //printf("%ld, created\n", pthread_self());
    int id = *(int*)arg;
    free(arg);
    const size_t records_size = sizeof(struct record) * records_to_read;
    do {
        //if(pthread_mutex_lock(&file_lock) == EOWNERDEAD) {
         //   pthread_mutex_consistent(&file_lock);
        //}
        struct record* records = calloc(records_to_read, sizeof(struct record));
        size_t bytes_total = 0;
        ssize_t bytes_cur = -1;
        while(bytes_total < records_size && !file_ended ) {
            bytes_cur = read(fd, records, records_size);
            //printf("%ld, read %d bytes\n", pthread_self(), (int)bytes_cur);
            if(bytes_cur == -1) {
                perror("read");
                break;
            }
            if(bytes_cur == 0) {
            file_ended = true;
            }
            bytes_total += bytes_cur;
        }
        //pthread_mutex_unlock(&file_lock);
        #if defined(ZADANIE1B)
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        #endif // defined
        for(int i = 0; i < records_to_read; i++) {
        char* text[RECORD_TEXT_LEN+1];
        memcpy(text, records[i].text, RECORD_TEXT_LEN);
        text[RECORD_TEXT_LEN] = '\0';
        //printf("%ld, scanning %d (%d) record\n", pthread_self(), i, records[i].id);
        if(strstr(records[i].text, searched) != NULL) {
            pthread_t self = pthread_self();
            printf("TID %ld (#%d): record %d\n", (long) self, id, records[i].id);
            #if defined(ZADANIE1A) || defined(ZADANIE1B)
            pthread_testcancel();
            if(pthread_mutex_lock(&thread_control) == EOWNERDEAD) {
                 pthread_mutex_consistent(&thread_control);
            }
            pthread_testcancel();
            for(int j = 0; j < threads_number; j++) {
                if(threads[j] != self) {
                    //printf("TID %ld (#%d): cancelling %ld (#%d)\n", (long) self, id, threads[j], j);

                    pthread_cancel(threads[j]);

                }
            }
            pthread_mutex_unlock(&thread_control);
            pthread_exit(EXIT_SUCCESS);
            #endif // defined
            }
        }
        #if defined(ZADANIE1B)
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        #endif // defined
    free(records);
    } while(!file_ended);
    pthread_exit(EXIT_SUCCESS);
}
struct timespec ms_to_timespec(long milisec)
{
    int sec = milisec / 1000;
    int nanosec = (milisec % 1000) * 1e6;
    struct timespec ts = {.tv_sec = sec, .tv_nsec = nanosec};
    return ts;
}


void sleep_ms(long milisec)
{
    struct timespec request = ms_to_timespec(milisec);
    struct timespec remaining;
    int ret = 0;
    do {
        ret = nanosleep(&request, &remaining);
        request = remaining;
    } while(ret == -1);
}

int main(int argc, char* argv[])
{
    if(argc != 5) {
        fprintf(stderr, "Required args: \n");
        fprintf(stderr, "number of threads, file name, number of records read consecutively, word to find");
        exit(-1);
    }
    threads_number = atoi(argv[1]);
    file_name = argv[2];
    records_to_read = atoi(argv[3]);
    searched = argv[4];

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);

    pthread_mutex_init(&file_lock, &attr);
    pthread_mutex_init(&thread_control, &attr);
    if((fd = open(file_name, O_RDONLY)) == -1) {
        perror("open");
        goto cleanup;
    }

    if((threads = calloc(threads_number, sizeof(pthread_t))) == NULL) {
        perror("malloc");
        goto cleanup;
    }
    if(pthread_mutex_lock(&thread_control) == EOWNERDEAD) {
        pthread_mutex_consistent(&thread_control);
    }
    for(int i = 0; i < threads_number; i++) {
        int ret;
        int* i_copy = malloc(sizeof(int));
        *i_copy = i;
        if((ret = pthread_create(threads+i, NULL, read_thread, i_copy))) {
            fprintf(stderr, "Cannot create thread #%d: %d\n", i, ret);
            threads[i] = 0;
            continue;
        }
        #ifdef ZADANIE1C
        if((ret = pthread_detach(threads[i]))) {
                fprintf(stderr, "Cannot detach thread #%d: %d\n", i, ret);;
            continue;
        }
        #endif
    }
    pthread_mutex_unlock(&thread_control);
    #ifndef ZADANIE1C
    for(int i = 0; i < threads_number; i++) {
        int ret;
        void* ret_thread;
        //printf("Joining thread %d %ldd\n", i, threads[i]);
        if((ret = pthread_join(threads[i], &ret_thread))) {
            fprintf(stderr, "Cannot join thread #%d: %d\n", i, ret);
            threads[i] = 0;
            continue;
        }
    }
    #endif // ZADANIE1C
    #ifdef ZADANIE1C
    sleep_ms(100);
    #endif
    cleanup:
    pthread_mutex_destroy(&thread_control);
    pthread_mutex_destroy(&file_name);
    if(threads) { free(threads); }
    if(fd != -1) { close(fd); }

    return 0;
}
