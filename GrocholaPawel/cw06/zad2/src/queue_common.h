#ifndef __QUEUE_COMMON_H_
#define __QUEUE_COMMON_H_
#include <stdbool.h>
#include <sys/types.h>
#include <mqueue.h>

#include "utils.h"
typedef enum message_t
{
    MSG_ECHO = 1,
    MSG_CAPS,
    MSG_TIME,
    MSG_STOP,
    MSG_REGISTER,
    MSG_RESPONSE,
    MSG_SERVER_STOPPED,
    MSG_CLIENT_STOPPED
} message_t;


#define QUEUE_SERVER_NAME "/ubjvbeirjvnerivjnworkvnekjrbnekfer"

size_t getmsglen(mqd_t queue);
int serverqueue_get();
ssize_t msg_get(mqd_t queue, OUT char** mtext, OUT message_t* msg_type);
void msg_send(mqd_t queue, long mtype, char* mtext, int priority);

#endif
