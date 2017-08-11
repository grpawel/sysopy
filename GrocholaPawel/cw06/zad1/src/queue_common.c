#include "queue_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
#include <string.h>

int serverqueue_get()
{
    key_t key = ftok(getenv("HOME"), QUEUE_ID_SERVER);
    if(key == -1) {
        perror("ftok");
    }
    int queue = msgget(key, IPC_CREAT | 0777);
    if(queue == -1) {
        perror("msgget");
        goto cleanup;
    }

    cleanup:
    return queue;
}

ssize_t msg_get(int server_queue_id, bool is_blocking, OUT struct msgbuf** msg, size_t maxlen)
{
    int flags = MSG_NOERROR;
    if(!is_blocking) {
        flags |= IPC_NOWAIT;
    }
    int msg_len = msgrcv(server_queue_id, *msg, maxlen, 0, flags);
    bool failed = msg_len == -1; //not necessarily error
    bool no_message = !is_blocking && errno == ENOMSG;
    if(failed && !no_message) {
        perror("msgrcv");
        ((struct msgbuf*) *msg)->mtype = 0;
    }
    if(failed && no_message) {
        ((struct msgbuf*) *msg)->mtype = 0;
    }
    return msg_len;
}

void msg_send(int queue_id, long type, char* text)
{
    int msg_len = strlen(text);
    if(msg_len == -1) {
        perror("strlen");
    }
    struct msgbuf msg;// = {.mtype = type, .mtext = text};
    msg.mtype = type;
    strcpy(msg.mtext, text);
    if(msgsnd(queue_id, &msg, msg_len, 0) == -1) {
        perror("msgsnd");
    }
}

void queue_delete(int queue_id)
{
    msgctl(queue_id, IPC_RMID, NULL);
}
