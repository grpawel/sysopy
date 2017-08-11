#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "misc.h"

#define OUT
struct prog
{
    char* name;
    char** args;
    int args_len;
};

//counts '|' characters to find numbre of programs
int count_progs(char** words, int words_len)
{
    int progs = 1;
    for(int i=0; i < words_len; i++) {
        if(strcmp(words[i], "|") == 0) {
            progs++;
        }
    }
    return progs;
}

void print_prog(struct prog* prog)
{
    printf("Prog: '%s', args:", prog->name);
    for(int i = 0; i < prog->args_len; i++) {
        printf(" '%s'", prog->args[i]);
    }
    printf("\n");

}

struct prog* prog_alloc(int name_i, int args_len, int args_start, char** words)
{
    struct prog* prog = malloc(sizeof(struct prog));
    prog->name = strdup(words[name_i]);
    prog->args_len = args_len + 1;
    prog->args = malloc(sizeof(char*) * (args_len + 2));
    prog->args[0] = strdup(prog->name);
    prog->args[args_len + 1] = NULL;
    for(int i=0; i < args_len; i++) {
        prog->args[i+1] = strdup(words[args_start + i]);
    }
    return prog;
}
void prog_free(struct prog* prog)
{
    for(int i=0; i < prog->args_len; i++) {
        free(prog->args[i]);
    }
    free(prog->args);
    free(prog->name);
    free(prog);
}

int find_progs(char** words, int words_len, OUT struct prog*** progs_out, OUT int* progs_len_out)
{
    int progs_len = count_progs(words, words_len);
    struct prog** progs = malloc(sizeof(struct prog*) * progs_len);
    if(!progs) {
        perror("malloc");
        return -1;
    }
    int current_prog = 0;
    bool is_arg = false;
    int name_start = 0;
    int args_start = 0;
    int args_len = 0;
    for(int i=0; i < words_len; i++) {
        if(strcmp(words[i], "|") == 0) {
            progs[current_prog] = prog_alloc(name_start, args_len, args_start, words);
            args_len = 0;
            current_prog++;
            is_arg = false;
        }
        else {
            if(!is_arg) {
                name_start = i;
                args_start = i+1;
                args_len = 0;
                is_arg = true;
            }
            else {
                args_len++;
            }
        }
    }
    progs[progs_len - 1] = prog_alloc(name_start, args_len, args_start, words);
    *progs_out = progs;
    *progs_len_out = progs_len;
    return 0;

}
void run_progs(struct prog** progs, int progs_len)
{
    int* fds;
    fds = malloc(sizeof(int) * progs_len * 2);
    for(int i = 0; i < progs_len; i+=2) {
        pipe(fds + i);
    }
    for(int i = 0; i < progs_len; i++) {
        int pid = fork();
        if(pid == 0) {
            if(i != 0) {
                dup2(fds[i*2 - 2], STDIN_FILENO);
            }
            if(i != progs_len - 1) {
                dup2(fds[i*2 + 1], STDOUT_FILENO);
            }
            execvp(progs[i]->name, progs[i]->args);
            exit(EXIT_SUCCESS);
        }
        if(pid < 0) {
            perror("fork");
        }
    }
    for(int i=0; i < progs_len; i++) {
        close(fds[i]);
    }
    free(fds);
}

void process_line(char* line)
{
    char** words = split_to_words(line);
    int words_len = count_words(line);
    struct prog** progs;
    int progs_len;
    find_progs(words, words_len, &progs, &progs_len);
    run_progs(progs, progs_len);
    for(int i=0; i < progs_len; i++) {
        prog_free(progs[i]);
    }
    free(progs);
}

int main(int argc, char *argv[])
{
    char* line = NULL;
    size_t len = 0;
    ssize_t nread;
    //for each line of input
    while ((nread = getline(&line, &len, stdin)) != -1) {
        process_line(line);
    }
    free(line);
    exit(EXIT_SUCCESS);
}
