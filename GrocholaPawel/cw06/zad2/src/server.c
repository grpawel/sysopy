#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include "queue_common.h"
#include "utils.h"

int client_queues_size = 10;
int next_assigned_id = 0;
mqd_t* client_queues = NULL;
bool* clients_active = NULL;
//returns id of client queue
int add_queue(char* queue_name);
mqd_t get_queue(int id);
int add_queue(char* queue_name)
{
    if(next_assigned_id >= client_queues_size) {
        mqd_t* new_client_queues;
        bool* new_clients_active;
        new_client_queues = realloc(client_queues, client_queues_size * 2 * sizeof(mqd_t));
        new_clients_active = realloc(clients_active, client_queues_size * 2 * sizeof(bool));
        if(new_client_queues && new_clients_active) {
            client_queues = new_client_queues;
            clients_active = new_clients_active;
            client_queues_size *= 2;

        }
        else {
            perror("realloc");
            return -1;
        }
    }
    int queue = mq_open(queue_name, O_WRONLY);
    if(queue == -1) {
        perror("mq_open");
    }
    client_queues[next_assigned_id] = queue;
    clients_active[next_assigned_id] = true;
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
    if(server_queue != -1) {
        for(int i=0; i < next_assigned_id; i++) {
            if(clients_active[i]) {
                msg_send(client_queues[i], MSG_SERVER_STOPPED, "", 5);
                mq_close(client_queues[i]);
            }
        }
        mq_close(server_queue);
        mq_unlink(QUEUE_SERVER_NAME);
    }
}

void handle_message(char* mtext, message_t msgtype)
{
    switch(msgtype) {
        case MSG_REGISTER: {
            char* client_queue_name = mtext;
            int id = add_queue(client_queue_name);
            char* id_str = int_to_chara(id);
            mqd_t queue = get_queue(id);
            msg_send(queue, MSG_RESPONSE, id_str, 1);
            printf("Registered client #%d\n", id);
            free(id_str);
            break;
        }
        case MSG_CLIENT_STOPPED: {
            int id = chara_to_int(mtext);
            clients_active[id] = false;
            mq_close(client_queues[id]);
            printf("Unregistered client #%d\n", id);
            break;
        }
        case MSG_ECHO: {
            int id = chara_to_int(mtext);
            char* str = mtext + 11;
            int queue = get_queue(id);
            msg_send(queue, MSG_RESPONSE, str, 1);
            printf("Echoing string '%s' to client #%d\n", str, id);
            break;
        }
        case MSG_CAPS: {
            int id = chara_to_int(mtext);
            char* str = strdup(mtext + 11);
            int i=0;
            while(str[i] != '\0') {
                str[i] = toupper(str[i]);
                i++;
            }
            int queue = get_queue(id);
            msg_send(queue, MSG_RESPONSE, str, 1);
            printf("Capitalized string '%s' for client #%d\n", str, id);
            free(str);
            break;
        }
        case MSG_TIME: {
            int id = chara_to_int(mtext);
            char str[20];
            time_t now = time(NULL);
            strftime(str, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
            int queue = get_queue(id);
            msg_send(queue, MSG_RESPONSE, str, 1);
            printf("Sent time '%s' to client #%d\n", str, id);
            break;
        }
        default:
            break;
    }
}
void messages_loop(int server_queue_id)
{
    bool is_stopping = false;
    bool stopped = false;
    char* mtext;
    message_t msg_type;
    do {
        if(is_stopping) {
            struct mq_attr attr;
            if(mq_getattr(server_queue_id, &attr) == -1) {
                perror("mq_getattr");
            }
            if(attr.mq_curmsgs == 0)
                break;
        }
        msg_get(server_queue_id, &mtext, &msg_type);

        switch(msg_type)
        {
            case MSG_STOP: {
                int id = chara_to_int(mtext);
                printf("Stopping server by request of client #%d\n", id);
                mq_close(client_queues[id]);
                clients_active[id] = false;
                is_stopping = true;
                break;
            }
            default:
                handle_message(mtext, msg_type);
                break;
        }
        free(mtext);
    } while(!stopped);
    for(int i=0; i < next_assigned_id; i++) {
    if(clients_active[i]) {
        msg_send(get_queue(i), MSG_SERVER_STOPPED, "", 5);
        }
    }
    return;

}
int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;
    client_queues = malloc(sizeof(mqd_t) * client_queues_size);
    clients_active = malloc(sizeof(bool) * client_queues_size);
    if(client_queues == NULL) {
        perror("malloc");
        goto cleanup;
    }
    int server_queue_id = serverqueue_get(O_RDONLY);
    if(server_queue_id == -1) {
        printf("Could not get message queue.");
        goto cleanup;
    }
    atexit(queue_server_delete);
    messages_loop(server_queue_id);

    cleanup:
    if(client_queues) free(client_queues);
    if(clients_active) free(clients_active);

    return 0;
}
