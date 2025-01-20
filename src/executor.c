#include "../include/tokens.h"
#include "../include/executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

static int count_items(struct token_item *first)
{
    int counter = 0;

    while(first != NULL) {
        counter++;
        first = first->next;
    }

    return counter;
}

static int count_cmd_args(char **cmdl)
{
    int counter = 0;
    
    while(*cmdl != NULL) {
        counter++;
        cmdl++;
    }

    return counter;
}

static int check_cd_case(struct token_item *first)
{
    if(first == NULL)
        return 0;

    return (strcmp(first->word, "cd") == 0) ? 1 : 0;
}

static int handle_cd_case(char **cmdl)
{
    int result;
    char *home;

    if(count_cmd_args(cmdl) > 2) {
        fprintf(stderr, "ERROR: try \"cd [PATH]\"\n");
        return -1;
    }

    if(*(cmdl + 1) == NULL) {
        home = getenv("HOME");
        if(home == NULL) {
            fprintf(stderr, "I don't know where's your home\n");
            return -1;
        }
        result = chdir(home);
    } else {
        result = chdir(*(cmdl + 1));
    }

    if(result == -1) 
        perror("chdir");
    
    return result;
}

static int handle_wait_result(int wr, int status)
{
    if(wr == -1) {
        perror("wait");
        return -1;
    } else {
        if(WIFEXITED(status)) {
            return 0;
        } else {
            return WTERMSIG(status);
        }
    }
}

static int is_background_process(struct token_item *first)
{
    while(first != NULL) {
        if(strcmp(first->word, "&") == 0 && first->type == separator)
            return 1;
        first = first->next;
    }
    return 0;
}

static char **make_cmd_line(struct token_item *first)
{
    char **cmd_line = NULL, **tmp;
    int counter;

    counter = count_items(first);
    cmd_line = malloc((sizeof(char **) * counter) + 1);
    tmp = cmd_line;

    while(first != NULL) {
        if(first->type == regular_token) {
            *tmp = first->word;
            tmp++;
        }
        first = first->next;
    }

    *tmp = NULL;

    return cmd_line;
}

void kill_zombies()
{
    int wr = 0, status;
    do
    {
        wr = wait4(-1, &status, WNOHANG, 0);
        if(wr != 0 && wr != -1) {
            printf("Background process with pid=%d finished.\n", wr);
        }
    } while (wr != 0 && wr != -1);
}

int exec_prog(struct token_item *first)
{
    int wr, status, pid;
    int result = 0;
    char **cmd_line;

    if(first == NULL) {
        result = 1;
        return result;
    }

    cmd_line = make_cmd_line(first);

    if(check_cd_case(first)) {
        result = handle_cd_case(cmd_line);
        free(cmd_line);
        return result;
    }

    kill_zombies();
    pid = fork();
    if(pid == 0) {
        /* child process */
        execvp(*cmd_line, cmd_line);
        perror(*cmd_line);
        exit(1);
    }
    
    /* parent process */
    if(!is_background_process(first)) {
        do {
            wr = wait(&status);
        } while(wr != pid);
        result = handle_wait_result(wr, status);
    } else {
        printf("New background process with pid=%d created\n", pid);
    }

	free(cmd_line);
    return result;
}
