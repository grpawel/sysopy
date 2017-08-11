#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "queue_common.h"

#define QUEUE_NAME_LEN 20
char queue_client_name[QUEUE_NAME_LEN];
mqd_t client_queue = -1;
mqd_t server_queue = -1;
int id = -1;
bool server_stopped = false;
char* teststring_prepare()
{
    static char teststring[] = "___ (random string)";
    for(int i=0; i < 3; i++) {
        char c = (char) random_incl('a', 'z');
        teststring[i] = c;
    }
    return teststring;
}

void queue_client_create()
{
    queue_client_name[0] = '/';
    for(int i=1; i < QUEUE_NAME_LEN; i++) {
        queue_client_name[i] = random_incl('a', 'z');
    }
    queue_client_name[QUEUE_NAME_LEN - 1] = '\0';
    client_queue = mq_open(queue_client_name, O_RDONLY | O_CREAT, 0777, NULL);
    if(client_queue == -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
}

void queue_client_delete()
{
    if(!server_stopped) {
        msg_send(server_queue, MSG_CLIENT_STOPPED, int_to_chara(id), 4);
        printf("[Client #%d] Unregistered.\n", id);
    }
    mq_close(client_queue);
    mq_unlink(queue_client_name);
}

void receive_id(mqd_t client_queue, int server_queue)
{
    msg_send(server_queue, MSG_REGISTER, queue_client_name, 3);
    char* mtext;
    message_t msg_type;
    msg_get(client_queue, &mtext, &msg_type);
    id = chara_to_int(mtext);
    printf("[Client #%d] Registered, got id.\n", id);
    if(mtext != NULL) free(mtext);
}

void send_messages(int client_queue, int server_queue, bool stop_server)
{
    bool stop = false;
    char* id_str = int_to_chara(id);
    int server_msg_len = getmsglen(server_queue);
    char* base_str = malloc(sizeof(char) * server_msg_len);
    memset(base_str, '\0', server_msg_len);
    strcpy(base_str, id_str);
    strcat(base_str, " ");
    char* mtext;
    char* str = malloc(sizeof(char) * server_msg_len);
    message_t msg_type;
    do {
        int choice = random_incl(1, 4);
        memset(str, '\0', server_msg_len);
        strcpy(str, base_str);
        switch(choice) {
            case 1: {
                strcat(str, teststring_prepare());
                msg_send(server_queue, MSG_ECHO, str, 1);
                msg_get(client_queue, &mtext, &msg_type);
                if(msg_type == MSG_SERVER_STOPPED) {
                    stop = server_stopped = true;
                    break;
                }
                printf("[Client #%d] Echoed string: '%s'\n", id, mtext);
                free(mtext);
                break;
            }
            case 2: {
                strcat(str, teststring_prepare());
                msg_send(server_queue, MSG_CAPS, str, 1);
                msg_get(client_queue, &mtext, &msg_type);
                if(msg_type == MSG_SERVER_STOPPED) {
                    stop = server_stopped = true;
                    break;
                }
                printf("[Client #%d] Capitalized string: '%s'\n", id, mtext);
                free(mtext);
                break;
            }
            case 3: {
                msg_send(server_queue, MSG_TIME, str, 1);
                msg_get(client_queue, &mtext, &msg_type);
                if(msg_type == MSG_SERVER_STOPPED) {
                    stop = server_stopped = true;
                    break;
                }
                printf("[Client #%d] Server time: '%s'\n", id, mtext);
                free(mtext);
                break;
            }
            case 4: {
                stop = true;
                if(stop_server) {
                    printf("[Client #%d] Attempting to stop server in 2 seconds...\n", id);
                    fflush(stdout);
                    sleep(2);
                    struct mq_attr attr;
                    if(mq_getattr(client_queue, &attr) == -1) {
                        perror("mq_getattr");
                    }
                    int msg_left = attr.mq_curmsgs;
                    bool done = false;
                    for(int i=0; i < msg_left && !done; i++) {
                        msg_get(client_queue, &mtext, &msg_type);
                        if(msg_type == MSG_SERVER_STOPPED) {
                            done = true;
                        }
                    }
                    server_stopped = true;
                    if(done) {
                        printf("[Client #%d] Server already stopped.\n", id);

                    }
                    else {
                        msg_send(server_queue, MSG_STOP, str, 4);
                        printf("[Client #%d] Server stopped.\n", id);
                    }
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
    bool stop_server = argc >= 2;
    queue_client_create();
    if(client_queue == -1) {
        fprintf(stderr, "Cannot register queue.\n");
        goto cleanup;
    }
    server_queue = serverqueue_get(O_WRONLY);
    if(server_queue == -1) {
        fprintf(stderr, "Cannot get server queue.\n");
        goto cleanup;
    }
    atexit(queue_client_delete);
    receive_id(client_queue, server_queue);
    srand(time(NULL) + id);
    send_messages(client_queue, server_queue, stop_server);

    cleanup:
    return 0;
}
