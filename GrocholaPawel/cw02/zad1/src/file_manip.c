#include "file_manip.h"
#include <stdbool.h>

//i, j  -record numbers
static error_t exchange_data(FileOper* foper, int i1, int i2)
{
    Record* r1;
    Record* r2;
    error_t err = ERR_SUCCESS;
    err = foper->get_record(foper, &r1, i1);
    if(err != ERR_SUCCESS) {
        return err;
    }
    err = foper->get_record(foper, &r2, i2);
    if(err != ERR_SUCCESS) {
        return err;
    }
    records_swap_data(r1, r2);
    err = foper->send_record(foper, r1);
    if(err == ERR_SUCCESS) {
        err = foper->send_record(foper, r2);
    }
    record_free(&r1);
    record_free(&r2);
    return err;
}

error_t file_shuffle(FileOper* foper)
{
    int n = foper->record_count;
    error_t err = ERR_SUCCESS;
    for(int i = 0; i < n - 2; i++) {
        int j = random_incl(i, n - 1);
        err = exchange_data(foper, i, j);
        if(err != ERR_SUCCESS) {
            break;
        }
    }
    return err;
}

error_t file_generate(FileOper* foper)
{
    FileOper* urandom;
    foper_open(&urandom, FS_LIB, foper->record_size, foper->record_count, "/dev/urandom", FILE_READ);
    Record* record;
    error_t err = ERR_SUCCESS;
    for(int i=0; i < foper->record_count; i++) {
        err = urandom->get_record(urandom, &record, i);
        if(err) {
            break;
        }
        err = foper->send_record(foper, record);
        record_free(&record);
        if(err) {
            break;
        }
    }
    if(urandom) {
        urandom->close(&urandom);
    }
    record_free(&record);
    return err;
}

static int record_compare(Record* lhs, Record* rhs)
{
    return (int) lhs->data[0] - (int) rhs->data[0];
}


error_t file_bubblesort(FileOper* foper)
{
    int n = foper->record_count;
    Record* r1;
    Record* r2;
    error_t err1 = ERR_SUCCESS;
    error_t err2 = ERR_SUCCESS;
    bool swapped;
    do {
        swapped = false;
        for(int i = 1; i < n; i++) {
            err1 = foper->get_record(foper, &r1, i-1);
            err2 = foper->get_record(foper, &r2, i);
            if(err1 || err2) {
                break;
            }
            if(record_compare(r1, r2) > 0) {
                records_swap_data(r1, r2);
                err1 = foper->send_record(foper, r1);
                err2 = foper->send_record(foper, r2);
                swapped = true;
            }
            if(err1 || err2) {
                break;
            }
            record_free(&r1);
            record_free(&r2);
        }
    } while(swapped);
    record_free(&r1);
    record_free(&r2);
    return err1 ? err1 : err2;
}
