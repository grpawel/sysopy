#ifndef __QUEUE_COMMON_H_
#define __QUEUE_COMMON_H_
#include <stdbool.h>
#include <sys/types.h>
#include "utils.h"
typedef enum message
{
    MSG_ECHO = 1,
    MSG_CAPS,
    MSG_TIME,
    MSG_STOP,
    MSG_REGISTER,
    MSG_RESPONSE
} message_t;


#define MSG_MAXLEN 64
#define QUEUE_ID_SERVER 3
#define QUEUE_ID_CLIENT_MIN 2


struct msgbuf
{
    long mtype;
    char mtext[MSG_MAXLEN];
};

int serverqueue_get();
void queue_delete(int queue_id);
ssize_t msg_get(int server_queue_id, bool is_blocking, OUT struct msgbuf** msg, size_t maxlen);
void msg_send(int queue_id, long mtype, char* mtext);

#endif
