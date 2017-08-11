#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
/*
Napisz program wypisujący kolejną literą alfabetu od A do Z.
 Po odebraniu sygnału SIGTSTP program zaczyna wypisywać litery wstecz.
 Po ponownym wysłaniu SIGTSTP program powraca do pierwotnego wypisywania.
 Program powinien umożliwiać wielokrotne przechodzenie przez powyższy cykl (sygnałem SIGTSTP).

Program powinien również obsługiwać sygnał SIGINT.
Po jego odebraniu program wypisuje komunikat "Odebrano sygnał SIGINT" i kończy działanie.
W kodzie programu, do przechwycenia sygnałów użyj zarówno funkcji signal,
jak i sigaction (np. SIGINT odbierz za pomocą signal, a SIGTSTP za pomocą sigaction).
*/
volatile sig_atomic_t got_sigtstp = false;

void handle_sigtstp(int unused_sig_no)
{
    got_sigtstp = true;
    signal(SIGTSTP, handle_sigtstp);
}

void handle_sigint(int unused_sig_no)
{
    printf("\nOdebrano sygnal SIGINT\n");
    exit(0);
}

void sleep_for_msec(int milisec)
{
    int sec = milisec / 1000;
    int nanosec = (milisec % 1000) * 1e6;
    struct timespec request = {.tv_sec = sec, .tv_nsec = nanosec};
    struct timespec remaining;
    int ret = 0;
    do {
        ret = nanosleep(&request, &remaining);
        request = remaining;
    } while(ret == -1);
}

void loop_print_chars()
{
    const int MSEC_BETWEEN_OUTPUTS = 500;
    char current = 'A'-1;
    bool dir_asc = true;
    for(;;)
    {
        if(got_sigtstp) {
            dir_asc = !dir_asc;
            got_sigtstp = false;
        }
        if(dir_asc) {
            current++;
            if(current > 'Z') current = 'A';
        }
        else {
            current--;
            if(current < 'A') current = 'Z';
        }
        printf("%c\n", current);
        sleep_for_msec(MSEC_BETWEEN_OUTPUTS);
    }
}

int main()
{
    signal(SIGTSTP, handle_sigtstp);
    struct sigaction siga = {.sa_handler = handle_sigint};
    sigemptyset(&siga.sa_mask);
    sigaction(SIGINT, &siga, NULL);
    loop_print_chars();

    return 0;
}
