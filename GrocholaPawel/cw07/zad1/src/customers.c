#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "barbershop.h"
#include "logger.h"
#include "queue.h"
#include "semaphores.h"
#include "shared_mem.h"
#include "utils.h"

static mutex_t access_waiting_room;
static semaphore_t customers_ready;

static int customers_count;
static int cuts_goal;
static queue_t waiting_room;

static memblock_t mem = NULL;
static semmut_data semmut_barb = NULL;
static semmut_data semmut_cust = NULL;

static void customer()
{
    pid_t pid = getpid();
    semmut_cust = semmut_init_private(1);
    if(!semmut_cust) {
        log_error("[Customer %d] Cannot create mutex, exiting.", pid);
    }
    mutex_t is_cut = mutex_create(semmut_cust);
    struct waiting_room_seat seat = {.pid = pid, .cut_customer = *is_cut};
    int cuts_done = 0;
    bool not_going_back = false;
    while(cuts_done < cuts_goal && !not_going_back) {
        log_debug("%d Wait for access to waiting room...", pid);
        mutex_wait(access_waiting_room);
        log_debug("%d Access to waiting room granted", pid);
        if(!queue_isempty(waiting_room)) {
            if(queue_len(waiting_room) == 0) {
                log_info("[Customer %d] Waking barber up.", pid);
            }
            else {
                log_info("[Customer %d] Going to waiting room.", pid);
            }
            log_debug("%d Inserting to queue", pid);
            queue_insert(waiting_room, &seat);
            log_debug("%d Signalling customer ready", pid);
            semap_signal(customers_ready);
            log_debug("%d Signalling waiting room accessible", pid);
            mutex_signal(access_waiting_room);
            log_debug("%d Waiting to be cut...", pid);
            mutex_wait(is_cut);
            cuts_done++;
            log_info("[Customer %d] Just had hair cut.", pid);
        }
        else {
            log_debug("%d Signalling waiting room accessible", pid);
            mutex_signal(access_waiting_room);
            log_info("[Customer %d] Waiting room full.", pid);
            not_going_back = true;
        }
    }
    log_info("[Customer %d] Exiting. Cuts: %d of %d.", pid, cuts_done, cuts_goal);
    exit(0);

}
static void customers()
{
    pid_t* children = calloc(customers_count, sizeof(pid_t));
    if(!children) {
        perror("calloc");
        goto cleanup;
    }
    for(int i = 0; i < customers_count; i++) {
        log_debug("Forking customer %d", i);
        pid_t child = fork();
        if(child > 0) {
            children[i] = child;
        }
        else if(child == 0) {
            customer();
        }
        else {
            perror("fork");
            goto cleanup;
        }
    }
    cleanup:
    for(int i = 0; i < customers_count; i++) {
        if(children[i] != 0) {
            waitid(P_PID, children[i], NULL, WEXITED);
        }
    }
    if(children) free(children);
}

static void cleanup(void)
{
    if(semmut_barb) {
        semmut_deinit(semmut_barb);
        semmut_barb = NULL;
    }
    if(semmut_cust) {
        semmut_deinit(semmut_cust);
        semmut_cust = NULL;
    }
    if(mem) {
        mem_close(mem);
        mem = NULL;
    }
}

int main(int argc, char* argv[])
{
    atexit(cleanup);
    log_init();
    //log_debug_enable();
    if(argc != 3) {
        log_error("Required args (2): K (number of customers), S (number of cuts of each customer)");
        exit(-1);
    }
    customers_count = atoi(argv[1]);
    if(customers_count <= 0) {
        log_error("Number of customers must be > 0");
        exit(-1);
    }
    cuts_goal = atoi(argv[2]);
    if(cuts_goal <= 0) {
        log_error("Number of cuts must be > 0");
        exit(-1);
    }

    semmut_barb = semmut_init(false, 2, SEM_FTOK_PATH, SEM_FTOK_ID);
    if(semmut_barb == NULL) {
        exit(-1);
    }
    access_waiting_room = mutex_create(semmut_barb);
    customers_ready = semap_create(semmut_barb);

    mem = mem_open(false, 0, MEM_FTOK_PATH, MEM_FTOK_ID);
    if(!mem) {
        log_error("Cannot access memory");
        exit(-1);
    }
    struct barbershop_data barbershop_data = fill_from_block(mem);
    waiting_room = barbershop_data.waiting_room;
    log_debug("Max size of queue: %d", waiting_room->len_max);

    customers();

    return 0;
}
