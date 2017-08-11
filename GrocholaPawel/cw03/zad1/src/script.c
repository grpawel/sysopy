#include "script.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include "misc.h"
#include "errors.h"
#ifdef ZADANIE_2
#include <sys/time.h>
rlim_t cpu_limit;
rlim_t as_limit;
#endif
static err_t run_command_child(char** argv)
{
#ifdef ZADANIE_2
    struct rlimit cpu_rlimit = {
        .rlim_cur = cpu_limit,
        .rlim_max = cpu_limit
        };
    if(setrlimit(RLIMIT_CPU, &cpu_rlimit) == -1)
        return ERR_CANNOT_SET_CPU_LIMIT;
    struct rlimit as_rlimit = {
        .rlim_cur = as_limit,
        .rlim_max = as_limit
        };
    if(setrlimit(RLIMIT_AS, &as_rlimit) == -1)
        return ERR_CANNOT_SET_AS_LIMIT;
#endif
    execvp(argv[0], argv);
    exit(ERR_COMMAND_CANNOT_RUN);
    return ERR_SUCCESS; //cannot happen
}

static err_t run_command(char* line)
{
    char** words = split_to_words(line);
    int args_count = count_words(line);
    char** argv = malloc(sizeof(char*) * (args_count+1));
    memcpy(argv, words, sizeof(char*) * args_count);
    argv[args_count] = NULL;

    pid_t pid = fork();
    if(pid == 0) {
        run_command_child(argv);
    }
    else if(pid >= 1) {
        siginfo_t info;
        waitid(P_PID, pid, &info, WEXITED);
        return info.si_status;
    }
    else { //pid <= -1
        return ERR_CANNOT_CREATE_CHILD_PROCESS;
    }
    return ERR_SUCCESS; //should not pass ifs
}

static err_t run_env_var(char* line)
{
    int words_count = count_words(line);
    char** words = split_to_words(line);
    switch(words_count) {
        case 1: {
            unsetenv(words[0]);
            return ERR_SUCCESS;
        }
        case 2: {
            setenv(words[0], words[1], true);
            return ERR_SUCCESS;
        }
        default: {
            return ERR_INVALID_VAR_DECL;
        }
    }
}

static err_t run_line(char* line)
{
    if(strlen(line) <= 1) {
        return ERR_SUCCESS;
    }
    if(line[0] == '#') {
        return run_env_var(line + 1);
    }
    else {
        return run_command(line);
    }
}
void run_script(FILE* file)
{
    char* buf = NULL;
    size_t len = 0;
    int line_num = 1;
    while(!feof(file)) {
        getline(&buf, &len, file);
        err_t ret = run_line(buf);
        if(ret != ERR_SUCCESS) {
            fprintf(stderr, "Error executing script line %d:\n%s\n", line_num, buf);
            if(ret >= ERROR_STARTING_NUM) {
                fprintf(stderr, "%s\n", err_to_string(ret));
                if(err_is_system(ret)) {
                    fprintf(stderr, "%s\n", strerror(errno));
                }

            }
            else {
                fprintf(stderr, "Command returned '%d'.\n", ret);
            }
            goto out;
        }
        ++line_num;
    }
out:
    free(buf);

}
