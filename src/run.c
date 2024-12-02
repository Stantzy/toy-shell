#include "../include/run.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>

static int check_cd(char *str)
{
    return (strcmp(str, "cd") == 0) ? 1 : 0;
}

static int count_args(char **args)
{
    int counter = 0;
    char **tmp = args;

    while(*tmp != NULL) {
        counter++;
        tmp++;
    }

    return counter;
}

static int handle_cd_case(char *name, char **args)
{
    int result;
    char *home;
    int local_argc;

    local_argc = count_args(args);

    if(local_argc > 2) {
        fprintf(stderr, "Specify the path for \'cd\'");
        return -1;
    }

    if(*args == NULL) {
        home = getenv("HOME");
        result = chdir(home);
    } else {
        result = chdir(*args);
    }

    if(result == -1) 
        perror("chdir");
    
    return result;
}

int run_program(char *name, char **args)
{
    int wr, status, pid;

    if(check_cd(name)) {
        handle_cd_case(name, args + 1);
        return 0;
    }

    pid = fork();
    if(pid == -1) {
        perror("fork");
        return -1;
	}
    if(pid == 0) {	/* child process */
        execvp(name, args);
        perror(name);
        exit(1);
    }
    /* parent proccess */
    wr = wait(&status);
    if(wr == -1) {
        perror("wait");
        return -1;
    } else {
        if(WIFEXITED(status))
            return 0;
        else
            return WTERMSIG(status);
    }
}