#ifndef __BARBERSHOP_H_
#define __BARBERSHOP_H_

#include <sys/types.h>

#include "shared_mem.h"
#include "queue.h"
#include "semaphores.h"


#define ACCESS_WAITING_ROOM_NAME "/qwertyuiopraexcrtvbyunmk"
#define CUSTOMERS_READY_NAME "/fvgyhbnjujmkokgaftyhji"
#define SEMMUT_NAME_LEN 10
#define MEMORY_BLOCK_NAME "/sxdesxcvgtfvbnjhyabnmkikjm"
struct waiting_room_seat
{
    pid_t pid;
    char cut_customer_mutex_name[SEMMUT_NAME_LEN + 1];
};

struct barbershop_data
{
    int* free_seats;
    queue_t waiting_room;
};

struct barbershop_data fill_from_block(memblock_t mem);

#define MEMORY_BLOCK_SIZE 20480

#endif
