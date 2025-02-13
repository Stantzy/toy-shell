#include "../../include/tokens.h"
#include "../../include/exec/exec_structs.h"
#include "../../include/exec/exec_options.h"

#include <stdlib.h>
#include <unistd.h>

int (*create_pipes(struct exec_options opt))[pipe_size]
{
    int (*ptr)[pipe_size] = NULL;

    if(opt.count_pipelines > 0) {
        ptr =
            malloc(sizeof(int[pipe_size]) * opt.count_pipelines); /* need to free */
        for(int i = 0; i < opt.count_pipelines; i++) {
            pipe(*(ptr + i));
        }
    }

    return ptr;
}

void make_pipelines(
    struct cmd_line *cl,
    struct exec_options opt,
    int (*ptr_pipe)[2]
)
{
    int i = 0;
    struct cmd_line *tmp = cl;
    int (*p_tmp)[2] = ptr_pipe;

    while(tmp != NULL) {
        if(i == 0) {
            tmp->fd_in = opt.cur_fdin;
            tmp->fd_out = p_tmp[i][1];
        } else if(tmp->next == NULL) {
            tmp->fd_in = p_tmp[i - 1][0];
            tmp->fd_out = opt.cur_fdout;
        } else {
            tmp->fd_in = p_tmp[i - 1][0];
            tmp->fd_out = p_tmp[i][1];
        }
        tmp = tmp->next;
        i++;
    }
}

void process_pipelines(
    int (*ptr_pipe)[2],
    struct cmd_line cl,
    struct exec_options opt
)
{
    for(int i = 0; i < opt.count_pipelines; i++) {
        if(ptr_pipe[i][0] != cl.fd_in && ptr_pipe[i][0] != cl.fd_out)
            close(ptr_pipe[i][0]);
        if(ptr_pipe[i][1] != cl.fd_in && ptr_pipe[i][1] != cl.fd_out)
            close(ptr_pipe[i][1]);
    }

    dup2(cl.fd_in, STDIN_FILENO);
    dup2(cl.fd_out, STDOUT_FILENO);
    
    for(int i = 0; i < opt.count_pipelines; i++) {
        if(ptr_pipe[i][0] == cl.fd_in || ptr_pipe[i][0] == cl.fd_out)
            close(ptr_pipe[i][0]);
        if(ptr_pipe[i][1] == cl.fd_in || ptr_pipe[i][1] == cl.fd_out)
            close(ptr_pipe[i][1]);
    }
}