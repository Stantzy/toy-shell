#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "../../include/tokens.h"
#include "../../include/exec/exec_structs.h"
#include "../../include/exec/exec_options.h"
#include "../../include/exec/handlers.h"
#include "../../include/exec/executor.h"
#include "../../include/exec/pipeline.h"
#include "../../include/exec/grpctrl.h"

static int check_cd_case(struct token_item *first)
{
    if(first == NULL)
        return 0;

    return (strcmp(first->word, "cd") == 0) ? 1 : 0;
}

static struct cmd_line *init_cmdl()
{
    struct cmd_line *item = NULL;

    item = malloc(sizeof(struct cmd_line));
    item->cmdl = (char**)malloc(sizeof(char *) * cmd_size);
    item->pid = 0;
    item->pgid = 0;
    item->fd_in = 0;
    item->fd_out = 1;
    item->next = NULL;
    
    return item;
}

static void free_cmdl(struct cmd_line *cl)
{
	struct cmd_line *tmp;

	while(cl != NULL) {
		tmp = cl->next;
        free(cl->cmdl);
		free(cl);
		cl = tmp;
	}
}

struct cmd_line *make_cmd_line(struct token_item *first)
{
    struct cmd_line *cl, *tmp;
    int offset = 0;

    cl = init_cmdl();
    tmp = cl;

    while(first != NULL) {
        if((first->type == separator && strcmp(first->word, "|") == 0) ||
        (first->type == separator && strcmp(first->word, ";") == 0)) {
            *(tmp->cmdl + offset) = NULL;
            tmp->next = init_cmdl();
            tmp = tmp->next;
            offset = 0;
            first = first->next;
            continue;
        }

        if(first->type == separator && strcmp(first->word, ">") == 0)
            break;
        if(first->type == separator && strcmp(first->word, ">>") == 0)
            break;
            
        if(first->type == regular_token) {
            *(tmp->cmdl + offset) = first->word;
            offset++;
        }

        first = first->next;
    }

    tmp->next = NULL;

    return cl;
}

int exec_prog(struct token_item *first)
{
    int pid;
    int result = 0, rdir_res = 0;
    int (*ptr_pipe)[2] = NULL;
    struct exec_options opt;
    struct cmd_line *cl, *tmp;

    if(first == NULL)
        return 1;

    init_options(&opt);
    if(update_options(first, &opt) == -1)
        return 2;

    cl = make_cmd_line(first);   /* need to free */
    ptr_pipe = create_pipes(opt);   /* need to free */

    if(check_cd_case(first) && opt.count_pipelines == 0) {
        result = handle_cd_case(cl->cmdl);
        goto ret;
    }

    rdir_res = handle_redirection(&opt);

    if(opt.count_pipelines > 0)
        make_pipelines(cl, opt, ptr_pipe);

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
    tmp = cl;
    sigchld_background_flag = opt.background;   /* global variable for SIGCHLD */
    while(tmp != NULL) {
        pid = fork();
        update_cmd_pgid(pid, &opt);
        if(pid > 0) {
            /* parent */
            tmp->pid = pid;
            tmp->pgid = opt.cmd_pgid;
            change_fg_proc(opt, *cl);
        
            if(opt.count_pipelines > 0)
                setpgid(pid, opt.cmd_pgid);
            else
                setpgid(pid, pid);

            dup2(opt.save_stdin, 0);
            dup2(opt.save_stdout, 1);
        }

        if(pid == 0) {
            /* child */
#ifdef DEBUG
            printf("###DEBUG: SID=%d, PPID=%d, PGID=%d, PID=%d\n",
                getsid(0),
                getppid(),
                getpgid(0),
                getpid()
            );
#endif
            process_pipelines(ptr_pipe, *tmp, opt);
            execvp(*(tmp->cmdl), tmp->cmdl);
            perror(*(tmp->cmdl));
            exit(1);
        }
        /* parent */
        if(opt.count_pipelines == 0 && tmp->next != NULL)   /* if ';' separator */
            waitpid(pid, NULL, 0);
        tmp = tmp->next;
    }

    /* parent */
    for(int i = 0; i < opt.count_pipelines; i++) {
        close(ptr_pipe[i][0]);
        close(ptr_pipe[i][1]);
    }

    handle_executed_processes(opt, cl);
    opt.cmd_pgid = getpid();
    change_fg_proc(opt, *cl);
ret:
	free_cmdl(cl);
    free(ptr_pipe);
    return result;
}