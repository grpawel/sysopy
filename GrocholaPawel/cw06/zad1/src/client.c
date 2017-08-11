#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "queue_common.h"

char* teststring_prepare()
{
    static char teststring[] = "___ (random string)";
    for(int i=0; i < 3; i++) {
        char c = (char) random_incl('a', 'z');
        teststring[i] = c;
    }
    return teststring;
}

int queue_client_create()
{
    int queue = msgget(IPC_PRIVATE, IPC_CREAT | 0777);
    if(queue == -1) {
        perror("msgget");
    }

    return queue;
}

int client_queue = -1;
void queue_client_delete()
{
    if(client_queue != -1)
        queue_delete(client_queue);
}
int receive_id(int client_queue, int server_queue, struct msgbuf* msg)
{
    char* client_queue_str = int_to_chara(client_queue);

    msg_send(server_queue, MSG_REGISTER, client_queue_str);
    msg_get(client_queue, true, &msg, MSG_MAXLEN);
    int id = chara_to_int(msg->mtext);
    printf("[Client #%d] Registered, got id.\n", id);
    if(client_queue_str != NULL) free(client_queue_str);
    return id;
}

void send_messages(int client_queue, int server_queue, int id, struct msgbuf* msg, bool stop_server)
{
    bool stop = false;
    char* id_str = int_to_chara(id);
    char base_str[MSG_MAXLEN];
    memset(base_str, '\0', MSG_MAXLEN);
    strcpy(base_str, id_str);
    strcat(base_str, " ");
    do {
        int choice = random_incl(1, 4);
        memset(msg->mtext, '\0', MSG_MAXLEN);
        char str[MSG_MAXLEN];
        strcpy(str, base_str);
        switch(choice) {
            case 1: {
                strcat(str, teststring_prepare());
                msg_send(server_queue, MSG_ECHO, str);
                msg_get(client_queue, true, &msg, MSG_MAXLEN);
                printf("[Client #%d] Echoed string: '%s'\n", id, msg->mtext);
                break;
            }
            case 2: {
                strcat(str, teststring_prepare());
                msg_send(server_queue, MSG_CAPS, str);
                msg_get(client_queue, true, &msg, MSG_MAXLEN);
                printf("[Client #%d] Capitalized string: '%s'\n", id, msg->mtext);
                break;
            }
            case 3: {
                msg_send(server_queue, MSG_TIME, str);
                msg_get(client_queue, true, &msg, MSG_MAXLEN);
                printf("[Client #%d] Server time: '%s'\n", id, msg->mtext);
                break;
            }
            case 4: {
                stop = true;
                if(stop_server) {
                    printf("[Client #%d] Stopping server in 2 seconds...\n", id);
                    fflush(stdout);
                    sleep(2);
                    msg_send(server_queue, MSG_STOP, str);
                }
                else {
                    printf("[Client #%d] Exiting...\n", id);
                }

            }
        }
    } while(!stop);
}
int main(int argc, char* argv[])
{
    (void) argv;
    bool stop_server = false;
    if(argc >= 2) {
        stop_server = true;
    }
    int server_queue = 0;
    int id;
    struct msgbuf* msg = malloc(sizeof(struct msgbuf));

    if(msg == NULL) {
        perror("malloc");
        goto cleanup;
    }
    if((client_queue = queue_client_create())== -1) {
        fprintf(stderr, "Cannot register queue.\n");
        goto cleanup;
    }
    if((server_queue = serverqueue_get()) == -1) {
        fprintf(stderr, "Cannot get server queue.\n");
        goto cleanup;
    }
    atexit(queue_client_delete);
    id = receive_id(client_queue, server_queue, msg);
    srand(time(NULL) + id);
    send_messages(client_queue, server_queue, id, msg, stop_server);

    cleanup:
    if(msg != NULL) free(msg);
    return 0;
}
