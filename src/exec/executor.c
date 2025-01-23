#include "../shared_include/tokens.h"
#include "exec_structs.h"
#include "exec_options.h"
#include "handlers.h"
#include "executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

static int count_items(struct token_item *first)
{
    int counter = 0;

    while(first != NULL) {
        counter++;
        first = first->next;
    }

    return counter;
}

static int check_cd_case(struct token_item *first)
{
    if(first == NULL)
        return 0;

    return (strcmp(first->word, "cd") == 0) ? 1 : 0;
}

static char **make_cmd_line(struct token_item *first)
{
    char **cmd_line = NULL, **tmp;
    int counter;

    counter = count_items(first);
    cmd_line = malloc((sizeof(char **) * counter) + 1);
    tmp = cmd_line;

    while(first != NULL) {
        if(first->type == separator && strcmp(first->word, ">") == 0)
            break;
        if(first->type == separator && strcmp(first->word, ">>") == 0)
            break;
            
        if(first->type == regular_token) {
            *tmp = first->word;
            tmp++;
        }
        first = first->next;
    }

    *tmp = NULL;

    return cmd_line;
}

int exec_prog(struct token_item *first)
{
    int pid, result = 0, rdir_res = 0;
    struct file_descriptors fd_info;
    struct exec_options opt;
    char **cmd_line;

    if(first == NULL) {
        result = 1;
        return result;
    }

    cmd_line = make_cmd_line(first);

    if(check_cd_case(first)) {
        result = handle_cd_case(cmd_line);
        goto ret;
    }

    init_options(&opt);
    update_options(first, &opt);
    rdir_res = handle_redirection(opt, &fd_info);

    if(rdir_res == -1) {
        fprintf(stderr, "Error: the output file was expected\n");
        result = 2;
        goto ret;
    }
    if(rdir_res == -2) {
        fprintf(stderr, "Error: the input file was expected\n");
        result = 3;
        goto ret;
    }

    signal(SIGCHLD, sigchld_handler);
    pid = fork();
    if(pid == 0) {
        /* child process */
        execvp(*cmd_line, cmd_line);
        perror(*cmd_line);
        exit(1);
    }
    
    /* parent process */
    dup2(fd_info.save_stdout, 1);
    dup2(fd_info.save_stdin, 0);

    result = handle_executed_process(opt, pid);

ret:
	free(cmd_line);
    return result;
}