#include "filefunc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
typedef struct _FileOperInternal
{
    //only one of these two is used simultaneously
    int fdescr;
    FILE* file;
    FunctionSet function_set;
} FileOperInternal;

const uint32_t FILE_READ = 1<<0;
const uint32_t FILE_WRITE = 1<<1;
const uint32_t FILE_CREATE = 1<<2;

Record* record_alloc()
{
    Record* record = malloc(sizeof(Record));
    return record;
}

void record_free(Record** record)
{
    if(record == NULL) {
        return;
    }
    if(*record != NULL) {
        free((*record)->data);
        free(*record);
    }
    *record = NULL;
}

Record* record_copy(Record* source)
{
    Record* cp = record_alloc();
    cp->size = source->size;
    cp->id = source->id;
    cp->data = malloc(source->size);
    memcpy(cp->data, source->data, source->size);
    return cp;
}

void records_swap_data(Record* r1, Record* r2)
{
    uint8_t* tmp_data = r1->data;
    ssize_t tmp_size = r1->size;
    r1->data = r2->data;
    r1->size = r2->size;
    r2->data = tmp_data;
    r2->size = tmp_size;
    return;
}

static error_t foper_lib_fopen(OUT FILE** out_file, const char* filename, uint32_t mode)
{
    FILE* pfile;
    char* mode_str;
    if(mode & FILE_CREATE) {
        FILE* ptemp = fopen(filename, "ab");
        uint8_t dummy = 1;
        fwrite(&dummy, 1, 1, ptemp);
        fclose(ptemp);
    }
    if((mode & FILE_READ) && (mode & FILE_WRITE)) {
        mode_str = "rb+";
    }
    else if(mode & FILE_READ) {
        mode_str  = "rb";
    }
    else if(mode & FILE_WRITE) {
        mode_str = "wb";
    }
    else {
        return ERR_WRONG_OPERATION;
    }
    pfile = fopen(filename, mode_str);
    if(pfile == NULL) {
        return ERR_CANNOTOPENFILE;
    }
    *out_file = pfile;
    return ERR_SUCCESS;
}

static error_t foper_lib_fseek(FileOper* foper, int record_id)
{
    FILE* pfile = foper->internal->file;
    int offset = record_id * foper->record_size;
    int res = fseek(pfile, offset, SEEK_SET);
    if(res != 0) {
        return ERR_CANNOTACCESSFILE;
    }
    return ERR_SUCCESS;
}

static error_t foper_lib_fread(FileOper* foper, OUT Record** out_record, int record_id)
{
    FILE* pfile = foper->internal->file;
    error_t err = foper_lib_fseek(foper, record_id);
    if(err != ERR_SUCCESS) {
        return err;
    }
    uint8_t buf[foper->record_size];
    size_t count = foper->record_size;
    size_t read_bytes = fread(buf, sizeof(uint8_t), count, pfile);
    if(read_bytes != count) {
        if(feof(pfile)) {
            return ERR_WRONGFILESIZE;
        }
        return ERR_CANNOTREADFILE;
    }
    Record* record = record_alloc();
    record->size = count;
    record->data = malloc(count);
    record->id = record_id;
    memcpy(record->data, buf, count);
    *out_record = record;
    return ERR_SUCCESS;
}

static error_t foper_lib_fwrite(FileOper* foper, Record* record)
{
    FILE *pfile = foper->internal->file;
    error_t err = foper_lib_fseek(foper, record->id);
    if(err != ERR_SUCCESS) {
        return err;
    }
    size_t count = foper->record_size;
    size_t bytes_wrote = fwrite(record->data, sizeof(uint8_t), count, pfile);
    if(bytes_wrote != count) {
        return ERR_CANNOTWRITEFILE;
    }
    return ERR_SUCCESS;
}

static error_t foper_readerror(FileOper* ignored0, OUT Record** ignored1, int ignored2)
{
    return ERR_WRONG_OPERATION;
}

static error_t foper_writeerror(FileOper* ignored0, Record* ignored1)
{
    return ERR_WRONG_OPERATION;
}

static error_t foper_lib_close(FileOper** foper)
{
    error_t err = ERR_SUCCESS;
    FILE *pfile = (*foper)->internal->file;
    int res = fclose(pfile);
    if(res != 0) {
        err = ERR_CANNOTCLOSEFILE;
    }
    free((*foper)->internal);
    free(*foper);
    *foper = NULL;
    return err;
}

static error_t foper_sys_open(OUT int *out_pfd, const char* filename, uint32_t mode)
{
    error_t err = ERR_SUCCESS;
    int pfd;
    int oflag = 0;
    //mode_t sys_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    if(mode & FILE_CREATE) {
        oflag |= O_CREAT;
    }
    if((mode & FILE_READ) && (mode & FILE_WRITE)) {
        oflag |= O_RDWR;
    }
    else if(mode & FILE_READ) {
        oflag |= O_RDONLY;
    }
    else if(mode & FILE_WRITE) {
        oflag |= O_WRONLY;
    }
    pfd = open(filename, oflag, 0777);
    if(pfd == -1) {
        err = ERR_CANNOTOPENFILE;
    }
    *out_pfd = pfd;
    return err;
}

static error_t foper_sys_lseek(FileOper* foper, int record_id)
{
    int pfd = foper->internal->fdescr;
    error_t err = ERR_SUCCESS;
    off_t offset = record_id * foper->record_size;
    off_t res = lseek(pfd, offset, SEEK_SET);
    if(res == -1) {
        err = ERR_CANNOTACCESSFILE;
    }
    return err;
}
static error_t foper_sys_read(FileOper* foper, OUT Record** out_record, int record_id)
{
    int pfd = foper->internal->fdescr;
    uint8_t buf[foper->record_size];
    ssize_t count = foper->record_size;
    error_t err = ERR_SUCCESS;
    err = foper_sys_lseek(foper, record_id);
    if(err != ERR_SUCCESS) {
        return err;
    }
    ssize_t read_bytes = read(pfd, buf, count);
    if(read_bytes != count) {
        if(read_bytes >= 0) {
            return ERR_WRONGFILESIZE;
        }
        return ERR_CANNOTREADFILE;
    }
    Record* record = record_alloc();
    record->size = count;
    record->data = malloc(count);
    record->id = record_id;
    memcpy(record->data, buf, count);
    *out_record = record;
    return ERR_SUCCESS;
}

static error_t foper_sys_write(FileOper* foper, Record* record)
{
    int pfd = foper->internal->fdescr;
    ssize_t count = record->size;
    error_t err = foper_sys_lseek(foper, record->id);
    if(err != ERR_SUCCESS) {
        return err;
    }
    ssize_t bytes_wrote = write(pfd, record->data, count); //might be a problem if record comes from different file
    if(bytes_wrote != count) {
        return ERR_CANNOTREADFILE;
    }
    return ERR_SUCCESS;
}

static error_t foper_sys_close(FileOper** foper)
{
    int pfd = (*foper)->internal->fdescr;
    int res = close(pfd);
    error_t err = ERR_SUCCESS;
    if(res == -1) {
        err = ERR_CANNOTCLOSEFILE;
    }
    free((*foper)->internal);
    free(*foper);
    *foper = NULL;
    return err;
}

error_t foper_open(OUT FileOper** out_foper,
        FunctionSet func_set, ssize_t record_size, int record_count, char* filename, uint32_t mode)
{
    FileOper* foper = malloc(sizeof(FileOper));
    foper->record_size = record_size;
    foper->record_count = record_count;
    FileOperInternal* internal = malloc(sizeof(FileOperInternal));
    foper->internal = internal;
    internal->function_set = func_set;
    error_t err = ERR_SUCCESS;
    switch(func_set) {
        case FS_LIB: {
            FILE *pfile;
            err = foper_lib_fopen(&pfile, filename, mode);
            internal->file = pfile;
            foper->get_record = foper_lib_fread;
            foper->send_record = foper_lib_fwrite;
            foper->close = foper_lib_close;
            break;
        }
        case FS_SYS: {
            int pfd;
            err = foper_sys_open(&pfd, filename, mode);
            internal->fdescr = pfd;
            foper->get_record = foper_sys_read;
            foper->send_record = foper_sys_write;
            foper->close = foper_sys_close;
            break;
        }
    }
    if(!(mode & FILE_READ)) {
        foper->get_record = foper_readerror;
    }
    if(!(mode & FILE_WRITE)) {
        foper->send_record = foper_writeerror;
    }
    if(err != ERR_SUCCESS) {
        free(foper);
        free(internal);
        return err;
    }
    *out_foper = foper;
    return ERR_SUCCESS;
}
