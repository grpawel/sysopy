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
#include <arpa/inet.h>
#define _DEFAULT_SOURCE
#include <endian.h>
#include <signal.h>
#include <netdb.h>

#include "utils.h"
#include "array.h"
#include "messages.h"

#define TOKENPASTE(x) #x
#define TOKENPASTE2(x) TOKENPASTE(x)
#define PERROR(x) do { if((x) == -1) {\
    perror(#x "\n("TOKENPASTE2(__FILE__) ": "TOKENPASTE2(__LINE__)")\n");\
    exit(EXIT_FAILURE);\
    } } while(0);
static int create_socket_local();
//static int create_socket_net(char* ip_string, int port);
static int create_socket_net2();
void unregister(void);
void sigint_handler(int signo);
bool registered = false;
int socket_client;
enum {CONNECTION_NET, CONNECTION_LOCAL} connection;

struct sockaddr address_server;
socklen_t addrlen_server = sizeof(address_server);

int main(int argc, char* argv[])
{
    signal(SIGINT, sigint_handler);
    atexit(unregister);
    if(argc < 3) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "%s ClientName123 local /tmp/socket\n", argv[0]);
        fprintf(stderr, "%s ClientName123 net 123.12.2.1 45654\n", argv[0]);
        exit(-1);
    }
    char client_name[CLIENT_NAME_LEN];
    strncpy(client_name, argv[1], CLIENT_NAME_LEN);
    client_name[CLIENT_NAME_LEN - 1] = '\0';
    if(strcmp("local", argv[2]) == 0) {
        connection = CONNECTION_LOCAL;
        char* socket_local_path = argv[3];
        socket_client = create_socket_local();
        printf("1\n");
        /*
        struct addrinfo hints;
        struct addrinfo *res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_LOCAL;
        hints.ai_socktype = SOCK_DGRAM;
        printf("2\n");
        getaddrinfo(socket_local_path, NULL, &hints, &res);
        printf("3\n");
        memcpy(&address_server, &res->ai_addr, sizeof(res->ai_addr));
        //memcpy(&addrlen_server, &res->ai_addrlen, sizeof(res->ai_addrlen));
        printf("4\n");
        freeaddrinfo(res);
        */
        struct sockaddr_un server_un;
        server_un.sun_family = AF_UNIX;
        strcpy(server_un.sun_path, socket_local_path);
        memcpy(&address_server, &server_un, sizeof(server_un));
    }
    else if(strcmp("net", argv[2]) == 0) {
        connection = CONNECTION_NET;
        char* ip_string = argv[3];
        char* port = argv[4];
        //printf("Connecting to %s:%s\n", ip_string, port);
        //socket_client = create_socket_net(ip_string, port);
        socket_client = create_socket_net2();
        /*
        struct addrinfo hints;
        struct addrinfo *res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        getaddrinfo(ip_string, port, &hints, &res);
        memcpy(&address_server, &res->ai_addr, sizeof(res->ai_addr));
        memcpy(&addrlen_server, &res->ai_addrlen, sizeof(res->ai_addrlen));
        freeaddrinfo(res);
        */

        struct sockaddr_in server_in;
        server_in.sin_family = AF_INET;
        inet_pton(AF_INET, ip_string, &server_in.sin_addr.s_addr);
        server_in.sin_port = htons(atoi(port));
        memcpy(&address_server, &server_in, sizeof(server_in));
        //strcpy(server_in.sin_path, ip);
        //*/

    }
    struct {
        message_enum type;
        char name[CLIENT_NAME_LEN];
    } register_message = {
        .type = MES_REGISTERING};
    printf("10\n");
    struct sockaddr address;
    socklen_t addrlen = sizeof(address);
    strcpy(register_message.name, client_name);
    PERROR(sendto(socket_client, &register_message, sizeof(register_message), 0, &address_server, addrlen_server));
    while(true) {
        message_t message;
        PERROR(recvfrom(socket_client, &message, sizeof(message), 0, &address, &addrlen));
        switch(message.type) {
            case MES_NAME_TAKEN: {
                printf("Name already taken (%s)\n", client_name);
                exit(EXIT_SUCCESS);
            }
            case MES_REGISTRATION_SUCCESS: {
                printf("Registered successfully\n");
                registered = true;
                break;
            }
            case MES_COMPUTE: {
                struct mes_compute* comp = (struct mes_compute*) &message;
                int num1 = comp->num1;
                int num2 = comp->num2;
                char oper = comp->oper;
                int task_id = comp->task_id;
                printf("Computing result of '%d %c %d' (#%d)\n", num1, oper, num2, task_id);
                int result;
                //sleep(3);
                switch(oper) {
                    case '+': result = num1 + num2; break;
                    case '-': result = num1 - num2; break;
                    case '*': result = num1 * num2; break;
                    case '/': result = num1 / num2; break;
                    default: result = -1;
                }
                struct mes_result res = {.type = MES_RESULT, .result = result, .task_id = task_id};
                PERROR(sendto(socket_client, &res, sizeof(res), 0, &address_server, addrlen_server));
                break;
            }
            case MES_PING: {
                PERROR(sendto(socket_client, &MES_DATA_PING_RESPONSE, sizeof(MES_DATA_PING_RESPONSE), 0, &address_server, addrlen_server));
                break;
            }
            default: {
                printf("Invalid message: %d\n", message.type);
                break;
            }
        }
    }

    //send(socket_client, )
    //cleanup:
    PERROR(shutdown(socket_client, SHUT_RDWR));
    PERROR(close(socket_client));
    return 0;
}


static int create_socket_local()
{
    int sock;/*
    // 0 - chosen automatically if only one exists
    PERROR(sock = socket(AF_LOCAL, SOCK_DGRAM, 0));
    struct sockaddr_un sockaddr_un;
    sockaddr_un.sun_family = AF_UNIX;
    strcpy(sockaddr_un.sun_path, socket_local_path);
    size_t sockaddr_un_len = sizeof(sockaddr_un.sun_family) + strlen(sockaddr_un.sun_path) + 1;
    //PERROR(connect(sock, (struct sockaddr*) &sockaddr_un, sockaddr_un_len));
    */
    PERROR(sock = socket(AF_UNIX, SOCK_DGRAM, 0));
    struct sockaddr_un sockaddr_un;
    sockaddr_un.sun_family = AF_UNIX;

    PERROR(bind(sock, (struct sockaddr*)&sockaddr_un, sizeof(sa_family_t)))
    return sock;
}
/*
static int create_socket_net(char* ip_string, int port)
{
    int sock;
    PERROR(sock = socket(AF_INET, SOCK_STREAM, 0));
    struct sockaddr_in sockaddr_in;
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(port);
    sockaddr_in.sin_addr.s_addr = inet_addr(ip_string);
    memset(sockaddr_in.sin_zero, '\0', sizeof(sockaddr_in.sin_zero));
    printf("connecting\n");
    PERROR(connect(sock, (struct sockaddr*) &sockaddr_in, sizeof(sockaddr_in)));

    return sock;
}
*/
static int create_socket_net2()
{
    int sock;/*
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    //hints.ai_flags = AI_PASSIVE;
    PERROR(getaddrinfo(ip_string, port, &hints, &res));
    */
    PERROR(sock = socket(AF_INET, SOCK_DGRAM, 0));
    //PERROR(connect(sock, res->ai_addr, res->ai_addrlen));
    //freeaddrinfo(res);
    return sock;
}

void unregister(void)
{
    if(registered) {
        printf("Unregistering..\n");
        PERROR(sendto(socket_client, &MES_DATA_UNREGISTERING, sizeof(MES_DATA_UNREGISTERING), MSG_NOSIGNAL, &address_server, addrlen_server));
        sleep_ms(100);
        }
}

void sigint_handler(int signo)
{
    (void) signo;
    //unregister();
    exit(0);
}
