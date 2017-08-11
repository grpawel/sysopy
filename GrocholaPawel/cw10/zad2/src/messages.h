#ifndef __MESSAGES_H_
#define __MESSAGES_H_

#define CLIENT_NAME_LEN 32
#define MESSAGE_LEN 1024
typedef enum message_enum
{
    MES_REGISTERING = 1,
    MES_UNREGISTERING,
    MES_NAME_TAKEN,
    MES_REGISTRATION_SUCCESS,
    MES_PING,
    MES_PING_RESPONSE,
    MES_COMPUTE,
    MES_RESULT
} message_enum;


typedef struct message_struct
{
    message_enum type;
    uint8_t buf[MESSAGE_LEN];
} message_t;


const struct {
    message_enum type;
} MES_DATA_NAME_TAKEN = {
    .type = MES_NAME_TAKEN};

const struct {
    message_enum type;
} MES_DATA_REGISTRATION_SUCCESS = {
    .type = MES_REGISTRATION_SUCCESS};

const struct {
    message_enum type;
} MES_DATA_UNREGISTERING = {
    .type = MES_UNREGISTERING};

const struct {
    message_enum type;
} MES_DATA_PING = {
    .type = MES_PING};

const struct {
    message_enum type;
} MES_DATA_PING_RESPONSE = {
    .type = MES_PING_RESPONSE};


struct mes_compute {
    message_enum type;
    int task_id;
    char oper;
    int num1;
    int num2;
};

struct mes_result {
    message_enum type;
    int result;
    int task_id;
};

#endif
