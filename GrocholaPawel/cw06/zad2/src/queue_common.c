#include "queue_common.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

size_t getmsglen(mqd_t queue)
{
    struct mq_attr attr;
    if(mq_getattr(queue, &attr) == -1) {
        perror("mq_getattr");
    }
    return attr.mq_msgsize;
}

mqd_t serverqueue_get(int oflag)
{
    mqd_t queue = mq_open(QUEUE_SERVER_NAME, oflag | O_CREAT, 0777, NULL);
    if(queue == -1) {
        perror("mq_open");
    }
    return queue;
}

ssize_t msg_get(mqd_t queue, OUT char** mtext, OUT message_t* msg_type)
{
    size_t buf_len = getmsglen(queue);
    char* buf = malloc(sizeof(char) * (buf_len + 1));
    if(buf == NULL) {
        perror("malloc");
    }
    ssize_t msg_len = mq_receive(queue, buf, buf_len + 1, NULL);
    if(msg_len == -1) {
        perror("mq_receive");
    }
    *mtext = malloc(sizeof(char) * msg_len);
    if(*mtext == NULL) {
        perror("malloc");
    }
    strcpy(*mtext, buf+1);
    *msg_type = (message_t) buf[0];
    //printf("Got message: '%s'\n", buf);
    free(buf);
    return msg_len;
}

void msg_send(mqd_t queue, long mtype, char* mtext, int priority)
{
    int mq_send(mqd_t mqdes, const char *msg_ptr,
              size_t msg_len, unsigned msg_prio);
    int msg_len = strlen(mtext);
    if(msg_len == -1) {
        perror("strlen");
    }
    int maxlen = getmsglen(queue);
    char* buf = malloc(sizeof(char) * maxlen);
    buf[0] = (char) mtype;
    strncpy(buf+1, mtext, maxlen - 2);
    buf[maxlen - 1] = '\0';
    if(mq_send(queue, buf, maxlen, priority) == -1) {
        perror("mq_send");
    }
    //printf("Sent message: '%s'\n", buf);
    return;
}
