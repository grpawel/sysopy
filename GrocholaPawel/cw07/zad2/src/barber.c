#include <assert.h>
#include <signal.h>
#include <stdlib.h>

#include <unistd.h>

#include "barbershop.h"
#include "logger.h"
#include "queue.h"
#include "semaphores.h"
#include "shared_mem.h"

static mutex_t access_waiting_room = NULL;
static semaphore_t customers_ready = NULL;
static mutex_t customer_cut = NULL;

static int* free_seats;
static queue_t waiting_room;

static memblock_t mem = NULL;

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
        log_debug("Signalling mutex %s", current_customer.cut_customer_mutex_name);
        customer_cut = mutex_create(current_customer.cut_customer_mutex_name, false, 0);
        mutex_signal(customer_cut);
        mutex_delete(customer_cut);
        log_info("Barber finished cutting hair of '%ld'.", current_customer.pid);
    }
}
static void cleanup(void)
{
    log_info("Exiting.");
    if(access_waiting_room) mutex_delete(access_waiting_room);
    if(customers_ready) semap_delete(customers_ready);
    if(customer_cut) mutex_delete(customer_cut);
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
    access_waiting_room = mutex_create(ACCESS_WAITING_ROOM_NAME, true, 1);
    if(!access_waiting_room) {
        log_error("Cannot get mutex.");
        exit(EXIT_FAILURE);
    }
    customers_ready = semap_create(CUSTOMERS_READY_NAME, true, 0);
    if(!customers_ready) {
        log_error("Cannot get semaphore.");
        exit(EXIT_FAILURE);
    }
    size_t shared_mem_size = 0;
    shared_mem_size += sizeof(int);
    shared_mem_size += queue_memsize(total_seats, sizeof(struct waiting_room_seat));
    if(shared_mem_size > MEMORY_BLOCK_SIZE) {
        log_error("Shared data cannot fit in shared memory.");
        exit(EXIT_FAILURE);
    }
    mem = mem_open(MEMORY_BLOCK_SIZE, MEMORY_BLOCK_NAME, true);
    if(!mem) {
        log_error("Cannot access memory");
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
