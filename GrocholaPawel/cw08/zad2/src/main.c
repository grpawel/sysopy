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


int sig;
int test;


struct record {
    int id;
    char text[RECORD_TEXT_LEN];
};

void sig_handler(int signo) {
    printf("Signal %d, PID %ld, TID %ld\n", signo, getpid(), pthread_self());
}

void* read_thread(void* arg)
{
    if(test == 3 || test == 5) {
        signal(sig, sig_handler);
    }
    if(test == 4) {
        signal(sig, SIG_IGN);
    }
    //printf("%ld, created\n", pthread_self());
    (void) arg;
    const size_t records_size = sizeof(struct record) * records_to_read;
    struct record* records = calloc(records_to_read, sizeof(struct record));
    do {
        if(pthread_mutex_lock(&file_lock) == EOWNERDEAD) {
            pthread_mutex_consistent(&file_lock);
        }
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
        pthread_mutex_unlock(&file_lock);
        for(int i = 0; i < records_to_read; i++) {
            //printf("%ld, scanning %d (%d) record\n", pthread_self(), i, records[i].id);
            if(strstr(records[i].text, searched) != NULL) {
                pthread_t self = pthread_self();
                printf("TID %ld: record %d\n", (long) self, records[i].id);
            }
        }
    }while(!file_ended);
    if(test == 6) {
        int a = 1;
        int b = 0;
        int c = a/b;
        printf("%d", c);
    }
    if(test == 3 || test == 4 || test == 5 || test == 6) {
        sleep(3);
        printf("TID %ld alive\n", pthread_self());
    }
    free(records);
    pthread_exit(EXIT_SUCCESS);
}
int main(int argc, char* argv[])
{
    if(argc != 7) {
        fprintf(stderr, "Required args: \n");
        fprintf(stderr, "number of threads, file name, number of records read consecutively, word to find\n");
        fprintf(stderr, "signo, test type\n");
        printf("SIGUSR1 %d, SIGTERM %d, SIGKILL %d, SIGSTOP %d\n", SIGUSR1, SIGTERM, SIGKILL, SIGSTOP);
        exit(-1);
    }
    sig = atoi(argv[5]);
    test = atoi(argv[6]);
    threads_number = atoi(argv[1]);
    file_name = argv[2];
    records_to_read = atoi(argv[3]);
    searched = argv[4];

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);

    pthread_mutex_init(&file_lock, &attr);
    if((fd = open(file_name, O_RDONLY)) == -1) {
        perror("open");
        goto cleanup;
    }

    if((threads = calloc(threads_number, sizeof(pthread_t))) == NULL) {
        perror("malloc");
        goto cleanup;
    }

    for(int i = 0; i < threads_number; i++) {
        int ret;
        if((ret = pthread_create(threads+i, NULL, read_thread, NULL))) {
            fprintf(stderr, "Cannot create thread #%d: %d\n", i, ret);
            threads[i] = 0;
            continue;
        }
        /*if((ret = pthread_detach(threads[i]))) {
            fprintf(stderr, "Cannot detach thread #%d: %d\n", i, ret);;
            continue;
        }*/
    }

    switch(test) {
        case 1: {
            kill(getpid(), sig);
            break;
        }
        case 2: {
            signal(sig, SIG_IGN);
            kill(getpid(), sig);
            break;
        }
        case 3: {
            sleep(1);
            signal(sig, sig_handler);
            kill(getpid(), sig);
            break;
        }
        case 4:
        case 5: {
            sleep(1);
            pthread_kill(threads[0], sig);
            break;
        }
    }
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
    cleanup:
    if(threads) { free(threads); }
    if(fd != -1) { close(fd); }

    return 0;
}
