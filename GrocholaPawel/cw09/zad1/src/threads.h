#ifndef __THREADS_H_
#define __THREADS_H_

void init_threads();
void deinit_threads();

void* reader_thread(void* args);

void* writer_thread(void* args);

#endif
