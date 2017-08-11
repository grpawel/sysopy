#include "threads.h"

#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "logger.h"
#include "utils.h"
#include "main.h"

void reader_read(int divider);
void writer_write();


pthread_mutex_t readers_waiting_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t readers_waiting_cond = PTHREAD_COND_INITIALIZER;
int readers_waiting = 0;

pthread_mutex_t writer_locked_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t writer_locked_cond = PTHREAD_COND_INITIALIZER;
bool writer_locked = false;

void* reader_thread(void* args)
{
    int divider = *((int*) args);
    log_debug("Created reader, divider: %d", divider);
    pthread_mutex_lock(&threads_creating_mutex);
    while(!threads_created) {
        pthread_cond_wait(&threads_creating_cond, &threads_creating_mutex);
    }
    pthread_mutex_unlock(&threads_creating_mutex);

    //business
    for(int cur_read = 0; cur_read < reads_to_do; cur_read++) {
        pthread_mutex_lock(&readers_waiting_mutex);
        readers_waiting++;
        log_debug("Reader: Waiting readers: %d", readers_waiting);
        pthread_cond_broadcast(&readers_waiting_cond);
        pthread_mutex_unlock(&readers_waiting_mutex);

        pthread_mutex_lock(&writer_locked_mutex);
        while(writer_locked) {
            log_debug("Trying to read, but writer is writing.");
            pthread_cond_wait(&writer_locked_cond, &writer_locked_mutex);
        }
        pthread_mutex_unlock(&writer_locked_mutex);

        reader_read(divider);

        pthread_mutex_lock(&readers_waiting_mutex);
        readers_waiting--;
        pthread_cond_broadcast(&readers_waiting_cond);
        pthread_mutex_unlock(&readers_waiting_mutex);
    }

    pthread_exit(EXIT_SUCCESS);
}

void* writer_thread(void* args)
{
    (void) args;
    log_debug("Created writer");
    pthread_mutex_lock(&threads_creating_mutex);
    while(!threads_created) {
        pthread_cond_wait(&threads_creating_cond, &threads_creating_mutex);
    }
    pthread_mutex_unlock(&threads_creating_mutex);

    //business
    for(int cur_write = 0; cur_write < writes_to_do; cur_write++) {
        pthread_mutex_lock(&readers_waiting_mutex);
        while(readers_waiting > 0) {
            log_debug("Writer: Readers waiting: %d", readers_waiting);
            pthread_cond_wait(&readers_waiting_cond, &readers_waiting_mutex);
        }
        pthread_mutex_unlock(&readers_waiting_mutex);
        pthread_mutex_lock(&writer_locked_mutex);
        while(writer_locked) {
            pthread_cond_wait(&writer_locked_cond, &writer_locked_mutex);
        }
        writer_locked = true;
        pthread_cond_broadcast(&writer_locked_cond);
        pthread_mutex_unlock(&writer_locked_mutex);

        //pthread_mutex_unlock(&readers_waiting_mutex);

        log_debug("writing...");
        writer_write();

        pthread_mutex_lock(&writer_locked_mutex);
        writer_locked = false;
        pthread_cond_broadcast(&writer_locked_cond);
        pthread_mutex_unlock(&writer_locked_mutex);
    }

    pthread_exit(EXIT_SUCCESS);
}

void init_threads() {}
void deinit_threads() {}
