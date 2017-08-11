#ifndef __MESSAGES_H_
#define __MESSAGES_H_

#define CLIENT_NAME_LEN 32
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
    uint32_t len;
    void* buf;
} message_t;

const struct {
    message_enum type;
    uint32_t len;
} MES_DATA_NAME_TAKEN = {
    .type = MES_NAME_TAKEN,
    .len = 0};

const struct {
    message_enum type;
    uint32_t len;
} MES_DATA_REGISTRATION_SUCCESS = {
    .type = MES_REGISTRATION_SUCCESS,
    .len = 0};

const struct {
    message_enum type;
    uint32_t len;
} MES_DATA_UNREGISTERING = {
    .type = MES_UNREGISTERING,
    .len = 0};

const struct {
    message_enum type;
    uint32_t len;
} MES_DATA_PING = {
    .type = MES_PING,
    .len = 0};

const struct {
    message_enum type;
    uint32_t len;
} MES_DATA_PING_RESPONSE = {
    .type = MES_PING_RESPONSE,
    .len = 0};


struct mes_compute {
    int task_id;
    char oper;
    int num1;
    int num2;
};


struct mes_result {
    int result;
    int task_id;
};


#endif
