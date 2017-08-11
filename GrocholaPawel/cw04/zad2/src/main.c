#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "utils.h"
#include "stack.h"
/*
Napisz program, który tworzy N potomków i oczekuje na ich prośby na przejście (sygnały SIGUSR1). Po uzyskaniu K próśb, proces powinien pozwolić kontynuować działanie wszystkim procesom, które poprosiły o przejście (wysłać im sygnał pozwolenia na rozpoczęcie pracy) i niezwłocznie akceptować każdą kolejną prośbę. Program powinien wypisać listę wszystkich otrzymanych sygnałów wraz numerem PID potomka oraz kodem zakończenia procesu (w przypadku otrzymania sygnału zakończenia pracy potomka).

Program kończy działanie po zakończeniu pracy ostatniego potomka albo po otrzymaniu sygnału SIGINT (w tym wypadku należy zakończyć wszystkie działające procesy potomne).

N i M są argumentami programu.

Zachowanie dzieci powinno być następujące. Każde dziecko najpierw symuluje pracę (uśnie na 0-10 sekund). Następnie wysyła sygnał SIGUSR1 prośby o przejście, a po uzyskaniu pozwolenia losuje jeden z 32 sygnałów czasu rzeczywistego  (SIGRTMIN,SIGRTMAX), wysyła go do rodzica i kończy działanie, zwracając wypisaną różnicę czasu między wysłaniem prośby a wysłaniem sygnału jako kod zakończenia procesu.
*/
const int seconds_sleep = 10;
void simulate_work(void)
{
    srand(time(NULL) ^ (getpid()<<16));
    int waittime_ms = random_incl(0, seconds_sleep*1000);
    //printf("Child %d waits for %d ms\n", getpid(), waittime_ms);
    sleep_ms(waittime_ms);
}

void send_to_parent(int signo)
{
    pid_t ppid = getppid();
    kill(ppid, signo);
}

volatile sig_atomic_t sig_pending_child = false;

void handle_sigusr2_child(int unused_signo)
{
    //printf("Child handler: PID: %d received signal: %d\n", getpid(), unused_signo);
    sig_pending_child = true;
    signal(SIGUSR2, handle_sigusr2_child);
}

void wait_for_signal_child(int signo)
{    //https://www.gnu.org/software/libc/manual/html_node/Sigsuspend.html#Sigsuspend
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset (&mask, signo);
    struct timespec waittime = {.tv_sec = seconds_sleep, .tv_nsec = 0};
    sigtimedwait(&mask, NULL, &waittime);
    //sig_pending_child = false;
}

void child_proc(void)
{
    signal(SIGUSR2, handle_sigusr2_child);
    simulate_work();
    long start_ms = current_ms();
    //printf("Child %d sends signal to parent.\n", getpid());
    send_to_parent(SIGUSR1);
    //printf("Child %d waits for signal form parent.\n", getpid());
    wait_for_signal_child(SIGUSR2);
    //printf("Child %d sends signal to parent.\n", getpid());
    send_to_parent(random_incl(SIGRTMIN, SIGRTMAX));
    long end_ms = current_ms();
    //printf("Child %d exits... Return val: %d\n", getpid(), end_ms - start_ms);
    exit((end_ms - start_ms) / 100);
}

struct childinfo
{
    pid_t pid;
    bool is_alive;
};

static struct childinfo **children;
static int children_total;
static int threshold; //TODO better name
volatile sig_atomic_t sig_pending_no = 0;
volatile sig_atomic_t sig_pending_pid;
volatile sig_atomic_t chld_return_value;

void handle_sig(int sig, siginfo_t *siginfo, void *unused_context)
{
    //printf("Handler: sig: %d pid: %d\n", sig, siginfo->si_pid);
    sig_pending_no = sig;
    sig_pending_pid = siginfo->si_pid;
    chld_return_value = siginfo->si_status;
}

void kill_all_children()
{
    for(int i=0; i < children_total; i++) {
        if(children[i]->is_alive && children[i]->pid != 0) {
            children[i]->is_alive = false;
            //printf("Killing child with PID %d", children[i]->pid);
            //kill(children[i]->pid, SIGKILL);
        }
    }
}
void main_proc(void)
{
    int children_alive = 0;
    int children_asked = 0;

    //printf("Inside main_proc (PID: %d) \n", getpid());
    //int i=0;
    for(int i=0; i < children_total; i++) {
        //printf("Just before fork (PID: %d)\n", getpid());
        pid_t pid = fork(); //WTF?????
        //printf("Just after fork (PID: %d)\n", getpid());
        if(pid < 0) {
            //perror("Cannot create child process\n");
            //TODO error and kill children
        }
        if(pid > 0) {
            //printf("Parent (%d) Created child: %d\n", getpid(), pid);
            children[i]->pid = pid;
            ++children_alive;
        }
        else if(pid == 0) {
            //printf("In newly created child: %d, parent: %d\n", getpid(), getppid());
            child_proc();
        }
    }

    bool allow_immediately = false;
    pid_t *children_asking = malloc(threshold * sizeof(pid_t));
    int i_asking = 0;

    sigset_t mask;
    sigemptyset(&mask);
    while(true) {
        //sigaddset (&mask, SIGUSR1);
        /* Wait for a signal to arrive. */
        struct timespec waittime = {.tv_sec = 2, .tv_nsec = 0};
        sigtimedwait(&mask, NULL, &waittime);
        if(sig_pending_no != 0) {
        //sigprocmask (SIG_UNBLOCK, &mask, NULL);
            printf("Received signal: %d from PID: %d", sig_pending_no, sig_pending_pid);
            if(sig_pending_no == SIGCHLD) {
                printf(" exit code: %d", chld_return_value);
                --children_alive;
                int status;
                waitpid(sig_pending_pid, &status, WNOHANG);
            }
            printf("\n");
        }
        if(sig_pending_no == SIGUSR1) {
            children_asking[children_asked] = sig_pending_pid;
            ++children_asked;
            if(!allow_immediately) {
                if(children_asked < threshold) {
                    children_asking[i_asking++] = sig_pending_pid;
                    //printf("Adding child %d to children (next i=%d). Check: %d\n", sig_pending_pid, i_asking, children_asking[i_asking]);
                }
                else {
                    allow_immediately = true;
                    for(int i=0; i < children_asked; i++) {
                        //printf("Sending SIGUSR2 to child %d\n", children_asking[i]);
                        kill(children_asking[i], SIGUSR2);
                    }
                }
            }
            else {
                //printf("Sending SIGUSR2 to child %d\n", children_asking[i]);
                kill(sig_pending_pid, SIGUSR2);
            }
        }
        else if(sig_pending_no == SIGINT) {
            kill_all_children();
            exit(0);
        }
        else if(SIGRTMIN <= sig_pending_no && sig_pending_no <= SIGRTMAX) {
            //do nothing
        }
        if(sig_pending_no == SIGALRM) {
            break;
        }
        sig_pending_no = 0;
    }
    //capture missing children
    while(children_alive > 0) {
        int status;
        waitpid(-1, &status, WNOHANG);
        if(WIFEXITED(status)) {
            printf("Did not catch signal, but a child has exited with status %d.\n", WEXITSTATUS(status));
            --children_alive;
        }
    }

}
int main(int argc, char* argv[])
{
/*
    printf("SIGUSR1: %d\n", SIGUSR1);
    printf("SIGUSR2: %d\n", SIGUSR2);
    printf("SIGCHLD: %d\n", SIGCHLD);
    printf("SIGRTMIN: %d\n", SIGRTMIN);
    printf("SIGRTMAX: %d\n", SIGRTMAX);
    printf("SIGINT: %d\n", SIGINT);
    */
    if(argc != 3) {
        perror("Wrong number of arguments.\n");
        perror("Required 2: number of child processes and threshold.\n");
        exit(-1);
    }
    children_total = atoi(argv[1]);
    threshold = atoi(argv[2]);

    children = malloc(children_total * sizeof(struct childinfo*));
    for(int i=0; i < children_total; i++) {
        children[i] = malloc(sizeof(struct childinfo));
        memset(children[i], '\0', sizeof(struct childinfo));
    }
    alarm(seconds_sleep+1);
    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_sigaction = &handle_sig;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    if(sigaction(SIGUSR1, &act, NULL)) perror("sigaction\n");
    if(sigaction(SIGINT, &act, NULL)) perror("sigaction\n");
    if(sigaction(SIGCHLD, &act, NULL)) perror("sigaction\n");
    if(sigaction(SIGUSR2, &act, NULL)) perror("sigaction\n");
    if(sigaction(SIGALRM, &act, NULL)) perror("sigaction\n");
    for(int sig = SIGRTMIN; sig <= SIGRTMAX; sig++)
        if(sigaction(sig, &act, NULL)) perror("sigaction\n");
//*/
    //signal(SIGUSR1, handler_TEMPORARY);
    //signal(SIGINT, handler_TEMPORARY);

    main_proc();


    for(int i=0; i < children_total; i++) {
        free(children[i]);
    }
    free(children);


    return 0;
}
