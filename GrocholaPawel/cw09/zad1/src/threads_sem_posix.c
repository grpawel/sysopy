#include "threads.h"

#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>

#include "logger.h"
#include "utils.h"
#include "main.h"
#include "queue.h"

void reader_read(int divider);
void writer_write();

int threads_count;
queue_t queue;
void* queue_mem;
struct queue_item {
    pthread_t thread;
    int wake_up_id;
    enum {THREAD_WRITER, THREAD_READER} type;
};

sem_t queue_access;
sem_t queue_added;
sem_t thread_finished_operation;
pthread_t overwatcher;
void* overwatch_thread(void* args);

sem_t* wake_ups;
sem_t access_wake_ups;
int next_assigned_wake_up = 0;
void init_threads()
{
    threads_count = readers_count + writers_count;
    queue_mem = malloc(queue_memsize(threads_count, sizeof(struct queue_item)));
    if(!queue_mem) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    queue = queue_create(threads_count, sizeof(struct queue_item), queue_mem);
    int ret;
    if((ret = sem_init(&queue_access, 0, 1))) {
        log_error("Cannot create semaphore: %d %s", ret, strerror(ret));
        exit(EXIT_FAILURE);
    }
    if((ret = sem_init(&queue_added, 0, 0))) {
        log_error("Cannot create semaphore: %d %s", ret, strerror(ret));
        exit(EXIT_FAILURE);
    }
    if((ret = sem_init(&thread_finished_operation, 0, 0))) {
        log_error("Cannot create semaphore: %d %s", ret, strerror(ret));
        exit(EXIT_FAILURE);
    }
    sem_init(&access_wake_ups, 0, 1);
    wake_ups = malloc(sizeof(sem_t) * threads_count);
    if(!wake_ups) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < threads_count; i++) {
        sem_init(wake_ups + i, 0, 0);
    }
    pthread_create(&overwatcher, NULL, overwatch_thread, NULL);
}

void deinit_threads()
{
    for(int i = 0; i < threads_count; i++) {
        sem_destroy(wake_ups + i);
    }
    sem_destroy(&access_wake_ups);
    free(wake_ups);
    free(queue_mem);
    pthread_cancel(overwatcher);
    sem_destroy(&queue_access);
    sem_destroy(&queue_added);
    sem_destroy(&thread_finished_operation);
}

void* reader_thread(void* args)
{
    int divider = *((int*) args);
    log_debug("Created reader, divider: %d", divider);

    int wake_up_id;
    sem_wait(&access_wake_ups);
    wake_up_id = next_assigned_wake_up++;
    sem_post(&access_wake_ups);

    pthread_mutex_lock(&threads_creating_mutex);
    while(!threads_created) {
        pthread_cond_wait(&threads_creating_cond, &threads_creating_mutex);
    }
    pthread_mutex_unlock(&threads_creating_mutex);

    struct queue_item item;
    //sem_init(&item.wake_up, 0, 0);
    item.thread = pthread_self();
    item.type = THREAD_READER;
    item.wake_up_id = wake_up_id;
    //business
    for(int i = 0; i < reads_to_do; i++) {
        sem_wait(&queue_access);
        log_debug("Inserting reader to queue, currently %d in queue.", queue_len(queue));
        queue_insert(queue, &item);
        sem_post(&queue_added);
        sem_post(&queue_access);
        sem_wait(wake_ups + wake_up_id);
        reader_read(divider);
        sem_post(&thread_finished_operation);
    }
    //sem_destroy(&item.wake_up);

    pthread_exit(EXIT_SUCCESS);
}

void* writer_thread(void* args)
{
    (void) args;
    log_debug("Created writer");

    int wake_up_id;
    sem_wait(&access_wake_ups);
    wake_up_id = next_assigned_wake_up++;
    sem_post(&access_wake_ups);

    pthread_mutex_lock(&threads_creating_mutex);
    while(!threads_created) {
        pthread_cond_wait(&threads_creating_cond, &threads_creating_mutex);
    }
    pthread_mutex_unlock(&threads_creating_mutex);


    struct queue_item item;
    //sem_init(&item.wake_up, 0, 0);

    item.thread = pthread_self();
    item.wake_up_id = wake_up_id;
    item.type = THREAD_WRITER;
    //business
    for(int i = 0; i < writes_to_do; i++) {
        sem_wait(&queue_access);
        log_debug("Inserting writer to queue, currently %d in queue.", queue_len(queue));
        queue_insert(queue, &item);
        sem_post(&queue_added);
        sem_post(&queue_access);
        sem_wait(wake_ups + wake_up_id);
        writer_write();
        sem_post(&thread_finished_operation);
    }
    //sem_destroy(&item.wake_up);

    pthread_exit(EXIT_SUCCESS);
}

void* overwatch_thread(void* args)
{
    (void) args;
    struct queue_item item;
    int readers_reading = 0;
    while(true) {
        log_debug("overwatcher: wait for sth in queue");
        sem_wait(&queue_added);
        sem_wait(&queue_access);
        queue_remove(queue, &item);
        log_debug("Overwatcher: removed %ld (%s) from queue", item.thread, item.type == THREAD_WRITER ? "WRITER" : "READER");
        sem_post(&queue_access);
        if(item.type == THREAD_WRITER) {
            while(readers_reading > 0) {
                sem_wait(&thread_finished_operation);
                readers_reading--;
            }
            sem_post(wake_ups + item.wake_up_id);
            sem_wait(&thread_finished_operation);
        }
        else if(item.type == THREAD_READER) {
            readers_reading++;
            sem_post(wake_ups + item.wake_up_id);
        }
        else {
            log_error("Should not happen!\n");
        }
    }
}
