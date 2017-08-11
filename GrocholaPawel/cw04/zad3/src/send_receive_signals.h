#ifndef __SEND_RECEIVE_SIGNALS_H
#define __SEND_RECEIVE_SIGNALS_H
#include <unistd.h>
#include <sys/types.h>
/*
extern int sigsend;
extern int sigdone;

typedef int (*sig_send_t)(__pid_t, int);
void send_signals(pid_t pid, int n, sig_send_t func_send);
void handle_sigsend(int unused_signo);
void handle_sigdone(int unused_signo);

int receive_signals();

int sigqueue2(pid_t pid, int sig);
*/
#endif
