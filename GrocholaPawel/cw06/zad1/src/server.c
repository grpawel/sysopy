#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "queue_common.h"
#include "utils.h"

int client_queues_size = 10;
int next_assigned_id = 0;
int* client_queues = NULL;
//returns id of client queue
int add_queue(int queue);
int get_queue(int id);

int add_queue(int queue)
{
    if(next_assigned_id >= client_queues_size) {
        int* new_queues;
        new_queues = realloc(client_queues, client_queues_size * 2 * sizeof(int));
        if(new_queues) {
            client_queues = new_queues;
            client_queues_size *= 2;
        }
        else {
            perror("realloc");
            return -1;
        }
    }
    client_queues[next_assigned_id] = queue;
    return next_assigned_id++;
}

int get_queue(int id)
{
    if(id < next_assigned_id) {
        return client_queues[id];
    }
    else {
        return -1;
    }
}

int server_queue = -1;
void queue_server_delete()
{
    if(server_queue != -1)
        queue_delete(server_queue);
}

void handle_message(struct msgbuf* msg)
{
    switch(msg->mtype) {
        case MSG_REGISTER: {
            int client_queue = chara_to_int(msg->mtext);
            int id = add_queue(client_queue);
            char* id_str = int_to_chara(id);
            msg_send(client_queue, MSG_RESPONSE, id_str);
            printf("Registered client #%d\n", id);
            free(id_str);
            break;
        }
        case MSG_ECHO: {
            int id = chara_to_int(msg->mtext);
            char* str = msg->mtext + 11;
            int queue = get_queue(id);
            msg_send(queue, MSG_RESPONSE, str);
            printf("Echoing string '%s' to client #%d\n", str, id);
            break;
        }
        case MSG_CAPS: {
            int id = chara_to_int(msg->mtext);
            char* str = strdup(msg->mtext + 11);
            int i=0;
            while(str[i] != '\0') {
                str[i] = toupper(str[i]);
                i++;
            }
            int queue = get_queue(id);
            msg_send(queue, MSG_RESPONSE, str);
            printf("Capitalized string '%s' for client #%d\n", str, id);
            free(str);
            break;
        }
        case MSG_TIME: {
            int id = chara_to_int(msg->mtext);
            char str[MSG_MAXLEN];
            time_t now = time(NULL);
            strftime(str, MSG_MAXLEN, "%Y-%m-%d %H:%M:%S", localtime(&now));
            int queue = get_queue(id);
            msg_send(queue, MSG_RESPONSE, str);
            printf("Sent time '%s' to client #%d\n", str, id);
            break;
        }
    }
}
void messages_loop(int server_queue_id)
{
    bool is_stopping = false;
    bool stopped = false;
    struct msgbuf* msg = malloc(sizeof(struct msgbuf));
    if(!msg) {
        perror("malloc");
        goto cleanup;
    }
    int last_len = 0;
    bool is_blocking = true;
    do {
        if(last_len > 0) memset(msg->mtext, '\0', MSG_MAXLEN);
        ssize_t msg_len = msg_get(server_queue_id, is_blocking, &msg, MSG_MAXLEN);
        if(msg_len == -1 && !is_stopping) continue;
        if(msg_len == -1 && is_stopping) stopped = true;
        switch(msg->mtype)
        {
            case MSG_STOP: {
                is_stopping = true;
                is_blocking = false;
                int id = chara_to_int(msg->mtext);
                printf("Stopping server by request of client #%d\n", id);
                break;
            }
            default:
                handle_message(msg);
                break;
        }
        last_len = msg_len;
    } while(!stopped);

    cleanup:
    if(msg) free(msg);
    return;

}
int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;
    client_queues = malloc(sizeof(int) * client_queues_size);
    if(client_queues == NULL) {
        perror("malloc");
        goto cleanup;
    }
    int server_queue_id = serverqueue_get();
    if(server_queue_id == -1) {
        printf("Could not get message queue.");
        goto cleanup;
    }
    atexit(queue_server_delete);
    messages_loop(server_queue_id);
    cleanup:
    if(client_queues) free(client_queues);

    return 0;
}
