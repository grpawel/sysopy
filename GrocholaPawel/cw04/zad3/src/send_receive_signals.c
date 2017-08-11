#include "send_receive_signals.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"
/*
int sigsend;
int sigdone;
static volatile sig_atomic_t received_signals_child = 0;
static volatile sig_atomic_t signals_done_child = false;

void handle_sigsend_child(int unused_signo)
{
    received_signals_child++;
    kill(getppid(), sigsend);
    signal(sigsend, handle_sigsend_child);
}

void handle_sigdone(int unused_signo)
{
    signals_done_child = true;
    signal(sigdone, handle_sigdone);
}


void send_signals(pid_t pid, int n, sig_send_t func_send)
{
    for(int i=0; i < n; i++) {
        func_send(pid, sigsend);
        //printf("signal %d\n", i);
    }
    func_send(pid, sigdone);
}

int receive_signals()
{
    int received = 0;
    while(!signals_done || sent_signals > 0) {
        if(sent_signals > 0) {
            sent_signals--;
            received++;
        }
    }
    signals_done = false;
    return received;
}

int sigqueue2(__pid_t pid, int sig)
{
    union sigval val = {.sival_int = 0};
    return sigqueue(pid, sig, val);
}
*/
