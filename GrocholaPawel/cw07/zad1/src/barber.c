#include <signal.h>
#include <stdlib.h>

#include <unistd.h>

#include "barbershop.h"
#include "logger.h"
#include "queue.h"
#include "semaphores.h"
#include "shared_mem.h"

static mutex_t access_waiting_room;
static semaphore_t customers_ready;
static const int SEMAPHORES_COUNT = 3;

static int* free_seats;
static queue_t waiting_room;

static memblock_t mem = NULL;
static semmut_data semmut = NULL;

static void barber()
{
    struct waiting_room_seat current_customer;
    while(true) {
        int customers_waiting = semap_get_state(customers_ready);
        if(customers_waiting == 0) {
            log_info("Barber goes to sleep.");
        }
        log_debug("Waiting for ready customers...");
        semap_wait(customers_ready);
        log_debug("Waiting for access to waiting room...");
        mutex_wait(access_waiting_room);
        log_debug("Access to waiting room granted.");
        *free_seats += 1;
        if(!queue_remove(waiting_room, &current_customer)) {
            log_error("No customer waiting but signaled otherwise.");
            break;
        }
        log_debug("Signalling waiting room accessible");
        mutex_signal(access_waiting_room);
        log_info("Barber cuts hair of customer '%ld'.", current_customer.pid);
        //sleep(1);
        mutex_signal(&(current_customer.cut_customer));
        log_info("Barber finished cutting hair of '%ld'.", current_customer.pid);
    }
}
static void cleanup(void)
{
    log_info("Exiting.");
    if(semmut) {
        semmut_deinit(semmut);
        semmut = NULL;
    }

    if(mem) {
        mem_close(mem);
        mem = NULL;
    }
}

void sigint_handler(int signo)
{
    (void) signo;
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    signal(SIGINT, sigint_handler);
    atexit(cleanup);
    log_init();
    //log_debug_enable();
    if(argc != 2) {
        log_error("Required args: N (number of seats in waiting room)");
        exit(EXIT_FAILURE);
    }
    int total_seats = atoi(argv[1]);
    if(total_seats < 0) {
        log_error("Number of seats in waiting room can not be less than 0");
        exit(EXIT_FAILURE);
    }
    semmut = semmut_init(true, 2, SEM_FTOK_PATH, SEM_FTOK_ID);
    if(semmut == NULL) {
        exit(EXIT_FAILURE);
    }
    access_waiting_room = mutex_create(semmut);
    customers_ready = semap_create(semmut);

    mutex_set_state(access_waiting_room, 1);
    semap_set_state(customers_ready, 0);

    size_t shared_mem_size = 0;
    shared_mem_size += sizeof(int);
    shared_mem_size += queue_memsize(total_seats, sizeof(struct waiting_room_seat));
    mem = mem_open(true, shared_mem_size, MEM_FTOK_PATH, MEM_FTOK_ID);
    if(!mem) {
        exit(EXIT_FAILURE);
    }
    struct barbershop_data barbershop_data = fill_from_block(mem);
    free_seats = barbershop_data.free_seats;
    waiting_room = barbershop_data.waiting_room;
    queue_create(total_seats, sizeof(struct waiting_room_seat), waiting_room);
    log_debug("Max size of queue: %d", waiting_room->len_max);

    barber();


    return EXIT_SUCCESS;
}
