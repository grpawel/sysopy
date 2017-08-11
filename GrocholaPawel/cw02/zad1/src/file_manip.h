#ifndef _FILE_MANIP_H
#define _FILE_MANIP_H
#include "filefunc.h"
#include "misc.h"

error_t file_shuffle(FileOper* foper);

error_t file_generate(FileOper* foper);

error_t file_bubblesort(FileOper* foper);
#endif
