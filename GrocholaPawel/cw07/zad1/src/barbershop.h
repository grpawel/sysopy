#ifndef __BARBERSHOP_H_
#define __BARBERSHOP_H_

#include <sys/types.h>

#include "shared_mem.h"
#include "queue.h"
#include "semaphores.h"

#define SEM_FTOK_PATH "/home"
#define SEM_FTOK_ID 2

#define MEM_FTOK_PATH "/home"
#define MEM_FTOK_ID 4

#define SIG_BARBER_CUTS SIGRTMIN

struct waiting_room_seat
{
    pid_t pid;
    struct mutex cut_customer;
};

struct barbershop_data
{
    int* free_seats;
    queue_t waiting_room;
};

struct barbershop_data fill_from_block(memblock_t mem);

#endif
