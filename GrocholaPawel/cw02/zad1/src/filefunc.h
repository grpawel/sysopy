#ifndef _FILEFUNC_H
#define _FILEFUNC_H
#include <sys/types.h>
#include <stdint.h>
#include "misc.h"

typedef struct _Record
{
    ssize_t size;
    uint8_t* data;
    int id;
} Record;

Record* record_alloc();
void record_free(Record** record);
Record* record_copy(Record* source);
void records_swap_data(Record* r1, Record* r2);

typedef struct _FileOperInternal FileOperInternal;

typedef struct _FileOper
{
    int record_count;
    int record_size;
    //out_record must be freed afterwards with record_free unless this function returned error
    error_t (*get_record)(struct _FileOper*, OUT Record** out_record, int record_id);
    error_t (*send_record)(struct _FileOper*, Record*);
    //closes file and destroys structure - not to use after this function
    error_t (*close)(struct _FileOper** foper);
    FileOperInternal* internal;
} FileOper;

typedef enum _FunctionSet
{
    FS_SYS,
    FS_LIB
} FunctionSet;

extern const uint32_t FILE_READ;
extern const uint32_t FILE_WRITE;
extern const uint32_t FILE_CREATE;

//opens file and creates structure
error_t foper_open(OUT FileOper** out_foper,
        FunctionSet func_set, ssize_t record_size, int record_count, char* filename, uint32_t mode);
#endif
