#include "barbershop.h"

#include <stdint.h>

struct barbershop_data fill_from_block(memblock_t mem)
{
    struct barbershop_data data;
    uint8_t* cur_byte = mem->block;
    data.free_seats = (int*) cur_byte;
    cur_byte += sizeof(int);
    data.waiting_room = (queue_t) cur_byte;
    return data;
}
