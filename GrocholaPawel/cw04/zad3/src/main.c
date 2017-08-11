#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
//#include "send_receive_signals.h"

typedef int (*sig_send_t)(__pid_t, int);
int sigqueue2(__pid_t pid, int sig);

int sigsend;
int sigdone;
volatile int received_signals_child = 0;
volatile int received_signals_parent = 0;
volatile bool signals_done_child = false;
volatile bool signals_done_parent = false;

void handle_sigsend_child(int unused_signo)
{
    //printf("Child: a signal.\n");
    received_signals_child++;
    //kill(getppid(), sigsend);
    //signal(sigsend, handle_sigsend_child);
}

void handle_sigdone_child(int unused_signo)
{
    signals_done_child = true;
    signal(sigdone, handle_sigdone_child);
}

void handle_sigsend_parent(int unused_signo)
{
    received_signals_parent++;
    //signal(sigsend, handle_sigsend_parent);
}

void handle_sigchld(int unused_signo)
{
    signals_done_parent = true;
}

void set_sigdone_handler(void (*handler)(int))
{
    struct sigaction act, old;
    //memset (&act, '\0', sizeof(act));
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    sigaddset (&act.sa_mask, sigsend);
    act.sa_flags = 0;
    if(sigaction(sigsend, &act, &old)) perror("sigaction\n");
}

void prog_common(int n, sig_send_t func_send)
{
    pid_t pid;
    pid = fork();

    if(pid < 0) {
        perror("fork");
    }
    if(pid == 0) {
        int resent_signals = 0;
        //signal(sigsend, handle_sigsend_child);
        set_sigdone_handler(handle_sigsend_child);
        signal(sigdone, handle_sigdone_child);
        while(!signals_done_child || resent_signals < received_signals_child) {
            if(resent_signals < received_signals_child) {
                kill(getppid(), sigsend);
                ++resent_signals;
            }
        }
        printf("Child: received %d signals, resending.\n", received_signals_child);
        exit(0);
    }
    if(pid > 0) {
        //signal(sigsend, handle_sigsend_parent);
        set_sigdone_handler(handle_sigsend_parent);
        signal(SIGCHLD, handle_sigchld);
        sleep(1);
        for(int i=0; i < n; i++) {
            kill(pid, sigsend);
        }
        printf("Parent: sent %d signals to child.\n", n);
        sleep(1);
        kill(pid, sigdone);
        while(!signals_done_parent) {}
        //sleep(1);
        printf("Parent: received %d signals.\n", received_signals_parent);
        waitpid(pid, NULL, 0);
    }
}

void prog0(void)
{
    perror("Not implemented");
}


int main(int argc, char* argv[])
{
    if(argc < 3) {
        perror("Required at least 2 arguments: Type(0,1,2,3) and N.\n");
        exit(EXIT_FAILURE);
    }
    int type = atoi(argv[1]);
    int n = atoi(argv[2]);
    switch(type) {
        case 0:
            prog0();
            break;
        case 1:
            sigsend = SIGUSR1;
            sigdone = SIGUSR2;
            prog_common(n, kill);
            break;
        case 2:
            sigsend = SIGUSR1;
            sigdone = SIGUSR2;
            prog_common(n, sigqueue2);
            break;
        case 3:
            sigsend = SIGRTMIN + 1;
            sigdone = SIGRTMIN + 2;
            prog_common(n, kill);
            break;

        default:
            perror("Invalid type\n");
            exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

int sigqueue2(__pid_t pid, int sig)
{
    union sigval val = {.sival_int = 0};
    return sigqueue(pid, sig, val);
}
