#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#define _DEFAULT_SOURCE
#include <endian.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>

#include "utils.h"
#include "array.h"
#include "messages.h"
#include "logger.h"

#define TOKENPASTE(x) #x
#define TOKENPASTE2(x) TOKENPASTE(x)
#define PERROR(x) do { if((x) == -1) {\
    perror(#x "\n("TOKENPASTE2(__FILE__) ": "TOKENPASTE2(__LINE__)")\n");\
    exit(EXIT_FAILURE);\
    } } while(0);

#define PERROR_SEND(x) do {\
    if((x) == -1 && errno != EPIPE) {\
    perror(#x "\n("TOKENPASTE2(__FILE__) ": "TOKENPASTE2(__LINE__)")\n");\
    exit(EXIT_FAILURE);\
    } } while(0);

typedef struct client {
    time_t last_active;
    int socket;
    bool to_unregister;
    char name[CLIENT_NAME_LEN];
} client_t;

bool should_exit = false;
#define UNREGISTER_TIME 5
pthread_mutex_t access_clients = PTHREAD_MUTEX_INITIALIZER;
array_t clients;


int socket_local;
int socket_net;

//int port;
//char* unix_path;

static int create_socket_local(const char* socket_local_path);
//static int create_socket_net(int port);
static int create_socket_net2(char* port);
static void connection_loop();
void handle_client_message(int socket_client);
int find_client_by_socket(array_t clients, int socket);
client_t* touch_client_by_socket(int socket);
void* tasks_loop(void* args);
void* ping_loop(void* args);
void sigint_handler(int signo);

int main(int argc, char* argv[])
{
    signal(SIGINT, sigint_handler);
    log_init();
    //log_enable(LOG_DEBUG);
    if(argc != 3) {
        fprintf(stderr, "Required args: port number and unix path:\n");
        fprintf(stderr, "%s 23230 /dfghjknbvcdtyuikmnb\n", argv[0]);
        exit(-1);
    }
    char* port = argv[1];
    char* socket_local_path = argv[2];

    array_create(&clients, sizeof(client_t), 10);

    /* create sockets */
    socket_local = create_socket_local(socket_local_path);
    socket_net = create_socket_net2(port);


    int ret;
    pthread_t tasks_thread;
    pthread_t ping_thread;
    if((ret = pthread_create(&tasks_thread, NULL, tasks_loop, NULL))) {
        log_error("Cannot create thread: %d %s", ret, strerror(ret));
    }
    pthread_detach(tasks_thread);
    if((ret = pthread_create(&ping_thread, NULL, ping_loop, NULL))) {
        log_error("Cannot create thread: %d %s", ret, strerror(ret));
    }
    pthread_detach(ping_thread);

    connection_loop();

    /* cleanup */

    PERROR(shutdown(socket_local, SHUT_RDWR));
    PERROR(shutdown(socket_net, SHUT_RDWR));
    PERROR(close(socket_local));
    PERROR(close(socket_net));
    PERROR(unlink(socket_local_path));

    return 0;
}

static int create_socket_local(const char* socket_local_path)
{
    int sock;
    // 0 - chosen automatically if only one exists
    PERROR(sock = socket(AF_LOCAL, SOCK_STREAM, 0));
    struct sockaddr_un sockaddr_un;
    sockaddr_un.sun_family = AF_UNIX;
    strcpy(sockaddr_un.sun_path, socket_local_path);
    size_t sockaddr_un_len = sizeof(sockaddr_un.sun_family) + strlen(sockaddr_un.sun_path) + 1;
    unlink(socket_local_path);
    PERROR(bind(sock, (struct sockaddr*) &sockaddr_un, sockaddr_un_len));

    PERROR(listen(sock, 10));
    return sock;
}
/*
static int create_socket_net(int port)
{
    int sock;
    PERROR(sock = socket(AF_INET, SOCK_STREAM, 0));
    struct sockaddr_in sockaddr_in;
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_addr.s_addr = INADDR_ANY;
    sockaddr_in.sin_port = htons(port);
    memset(sockaddr_in.sin_zero, '\0', sizeof(sockaddr_in.sin_zero));
    PERROR(bind(sock, (struct sockaddr*) &sockaddr_in, sizeof(sockaddr_in)));

    PERROR(listen(sock, 10));
    return sock;
}
*/
static int create_socket_net2(char* port)
{
    int sock;
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port, &hints, &res);

    PERROR(sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol));
    PERROR(bind(sock, res->ai_addr, res->ai_addrlen));
    PERROR(listen(sock, 10));
    freeaddrinfo(res);
    return sock;
}

static void connection_loop()
{
    int epollfd;
    #define MAX_EVENTS 10
    struct epoll_event event, events[MAX_EVENTS];
    PERROR(epollfd = epoll_create1(0));
    epoll_data_t event_data = {.fd = socket_local};
    event.events = EPOLLIN;
    event.data = event_data;
    PERROR(epoll_ctl(epollfd, EPOLL_CTL_ADD, socket_local, &event));
    event.data.fd = socket_net;
    PERROR(epoll_ctl(epollfd, EPOLL_CTL_ADD, socket_net, &event));

    while(!should_exit) {
        int events_count;
        PERROR(events_count = epoll_wait(epollfd, events, MAX_EVENTS, -1));
        //log_debug("Got %d events", events_count);
        for(int i = 0; i < events_count; i++) {
            log_debug("socket local: %d net: %d", socket_local, socket_net);
            //log_debug("EPOLLIN %d EPOLLOUT %d EPOLLRDHUP %d EPOLLPRI %d", EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI);
            //log_debug("EPOLLERR %d EPOLLHUP %d EPOLLET %d EPOLLONESHOT %d", EPOLLERR, EPOLLHUP, EPOLLET, EPOLLONESHOT);
            if(events[i].data.fd == socket_local) {
                log_debug("Event on local socket");
                int socket_client;
                struct sockaddr_un sockaddr_client;
                socklen_t sockaddr_client_size = sizeof(sockaddr_client);
                PERROR(socket_client = accept(socket_local, (struct sockaddr*) &sockaddr_client, &sockaddr_client_size));
                event.data.fd = socket_client;
                PERROR(epoll_ctl(epollfd, EPOLL_CTL_ADD, socket_client, &event));
            }
            else if(events[i].data.fd == socket_net) {
                log_debug("Event on net socket");
                int socket_client;
                struct sockaddr_in sockaddr_client;
                socklen_t sockaddr_client_size = sizeof(sockaddr_client);
                PERROR(socket_client = accept(socket_net, (struct sockaddr*) &sockaddr_client, &sockaddr_client_size));
                event.data.fd = socket_client;
                PERROR(epoll_ctl(epollfd, EPOLL_CTL_ADD, socket_client, &event));
            }
            else if(events[i].events == EPOLLIN){
                log_debug("Message from client");
                handle_client_message(events[i].data.fd);
            }
            else {
                log_debug("epoll fd %d epoll event %d", events[i].data.fd, events[i].events);
                //char buf;
                PERROR(epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &event));
                close(events[i].data.fd);
                //read(events[i].data.fd, &buf, 1);
            }
        }
    }
}

void handle_client_message(int socket_client)
{
    message_enum msg_type;
    uint32_t msg_size;
    void* buf;
    int ret;
    PERROR(ret = recv(socket_client, &msg_type, sizeof(msg_type), 0));
    if(ret == 0) return;
    PERROR(ret = recv(socket_client, &msg_size, sizeof(msg_size), 0));
    if(ret == 0) return;
    if(msg_size > 0) {
        buf = malloc(msg_size);
        PERROR(recv(socket_client, buf, msg_size, 0));
    }

    switch(msg_type) {
        case MES_REGISTERING: {
            char* client_name = buf;
            bool is_name_unique = true;
            pthread_mutex_lock(&access_clients);
            for(int i = 0; i < array_len(clients); i++) {
                client_t* client = array_get(clients, i);
                if(strcmp(client->name, client_name) == 0) {
                    is_name_unique = false;
                    break;
                }
            }
            pthread_mutex_unlock(&access_clients);
            if(is_name_unique) {
                client_t client;
                time(&client.last_active);
                strcpy(client.name , client_name);
                client.socket = socket_client;
                client.to_unregister = false;
                pthread_mutex_lock(&access_clients);
                printf("Registered client '%s'\n", client_name);
                PERROR_SEND(send(socket_client, &MES_DATA_REGISTRATION_SUCCESS, sizeof(MES_DATA_REGISTRATION_SUCCESS), MSG_NOSIGNAL));
                array_insert_last(clients, &client);
                pthread_mutex_unlock(&access_clients);
            }
            else {
                printf("Client '%s' tried to register but another one exists.\n", client_name);
                PERROR_SEND(send(socket_client, &MES_DATA_NAME_TAKEN, sizeof(MES_DATA_NAME_TAKEN), MSG_NOSIGNAL));
                shutdown(socket_client, SHUT_RDWR);
                close(socket_client);
            }
            break;
        }
        case MES_PING_RESPONSE: {
            touch_client_by_socket(socket_client);
            break;
        }
        case MES_UNREGISTERING: {
            pthread_mutex_lock(&access_clients);
            int i = find_client_by_socket(clients, socket_client);
            client_t* client = array_get(clients, i);
            printf("Client '%s' unregistered.\n", client->name);
            shutdown(client->socket, SHUT_RDWR);
            close(client->socket);
            array_remove(clients, i);
            pthread_mutex_unlock(&access_clients);
            break;
        }
        case MES_RESULT: {
            client_t* client = touch_client_by_socket(socket_client);
            if(client == NULL) return;
            struct mes_result* res = buf;
            printf("Result of #%d computed by '%s': %d\n", res->task_id, client->name, res->result);
            break;
        }
        default:
            log_error("Got invalid message.");
            break;
    }
    if(msg_size > 0) {
        free(buf);
    }
}

int find_client_by_socket(array_t clients, int socket)
{
    for(int i = 0; i < array_len(clients); i++) {
        client_t* client = array_get(clients, i);
        if(client->socket == socket) {
            return i;
        }
    }
    return -1;
}

client_t* touch_client_by_socket(int socket_client)
{
    pthread_mutex_lock(&access_clients);
    int i = find_client_by_socket(clients, socket_client);
    pthread_mutex_unlock(&access_clients);
    if(i == -1) {
        fprintf(stderr, "Got message from unregistered client?!\n");
        return NULL;
    }
    client_t* client = array_get(clients, i);
    time(&client->last_active);
    return client;
}

void* tasks_loop(void* args)
{
    log_debug("thread started");
    (void) args;
    int task_id = 0;
    char oper;
    int num1;
    int num2;
    srand(time(NULL));
    while(!should_exit) {
        scanf("%d %c %d", &num1, &oper, &num2);
        pthread_mutex_lock(&access_clients);
        if(array_len(clients) == 0) {
            printf("No registered clients!\n");
            continue;
        }
        int client_i = random_incl(0, array_len(clients) - 1);
        client_t* client = array_get(clients, client_i);
        pthread_mutex_unlock(&access_clients);
        struct mes_compute mes_compute;
        message_enum mes_type = MES_COMPUTE;
        uint32_t mes_size = sizeof(mes_compute);
        mes_compute.task_id = task_id;
        mes_compute.num1 = num1;
        mes_compute.num2 = num2;
        mes_compute.oper = oper;
        printf("Sending '%d %c %d' (#%d) to client '%s'\n", num1, oper, num2, task_id, client->name);
        task_id++;
        int ret = 0;
        PERROR_SEND(send(client->socket, &mes_type, sizeof(mes_type), MSG_NOSIGNAL));
        PERROR_SEND(send(client->socket, &mes_size, sizeof(mes_size), MSG_NOSIGNAL));
        //printf("type: %d, size: %d\n", mes_type, mes_size);
        PERROR(ret = send(client->socket, &mes_compute, sizeof(mes_compute), MSG_NOSIGNAL));
        //printf("Sent %d bytes.\n", ret);
    }
    return NULL;
}

void* ping_loop(void* args)
{
    (void) args;
    time_t now;
    while(!should_exit) {
        pthread_mutex_lock(&access_clients);
        time(&now);
        for(int i = 0; i < array_len(clients); i++) {
            client_t* client = array_get(clients, i);
            if(now - client->last_active >= UNREGISTER_TIME) {
                printf("Client '%s' not active for at least %d seconds - unregistering\n", client->name, UNREGISTER_TIME);
                array_remove(clients, i);
                i--;
            }
        }
        pthread_mutex_unlock(&access_clients);
        sleep(UNREGISTER_TIME - 1);
        pthread_mutex_lock(&access_clients);
        for(int i = 0; i < array_len(clients); i++) {
            client_t* client = array_get(clients,  i);
            PERROR_SEND(send(client->socket, &MES_DATA_PING, sizeof(MES_DATA_PING), MSG_NOSIGNAL));
        }
        pthread_mutex_unlock(&access_clients);
    }
    return NULL;
}

void sigint_handler(int signo)
{
    (void) signo;
    should_exit = true;
}
