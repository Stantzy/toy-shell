#include "../../include/exec/exec_structs.h"
#include "../../include/exec/handlers.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

enum { int_buff = 11 };

static int count_cmd_args(char **cmdl)
{
    int counter = 0;
    
    while(*cmdl != NULL) {
        counter++;
        cmdl++;
    }

    return counter;
}

int handle_cd_case(char **cmdl)
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

static int no_redirections(struct exec_options opt)
{
    return opt.rdir_out_flag == 0 &&
        opt.rdir_append_flag == 0 &&
        opt.rdir_in_flag == 0;
}

#ifdef DEBUG
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
#endif

int handle_executed_process(struct exec_options opt, struct cmd_line *cl)
{
    int wr, status;
    int result = 0;

    if(opt.background) {
        while(cl != NULL) {
            printf("New background process with pid=%d created\n", cl->pid);
            cl = cl->next;
        }
    } else {
        while((wr = wait4(-1, &status, 0, NULL)) > 0) {
#ifdef DEBUG            
            if(wr > 0) {
                printf("Process with pid=%d completed ", wr);
                result = handle_wait_result(wr, status);
                printf("with result=%d\n", result);
            }
#endif            
        }

        
    }

    return result;
}

int handle_redirection(struct exec_options *opt)
{
    int out_flags = 0;

    opt->save_stdin = dup(STDIN_FILENO);    
    opt->save_stdout = dup(STDOUT_FILENO);
    opt->cur_fdin = opt->save_stdin;
    opt->cur_fdout = opt->save_stdout;

    if(no_redirections(*opt))
        return 0;

    if(opt->rdir_in_flag) {
        opt->cur_fdin = open(opt->in_path, O_RDONLY);
        if(opt->cur_fdin == -1) {
            perror("Error opening the input file");
            return -1;
        }
        dup2(opt->cur_fdin, 0);
    }
    if(opt->rdir_out_flag)
        out_flags = O_CREAT | O_RDWR | O_APPEND;
    else
    if(opt->rdir_append_flag)
        out_flags = O_CREAT | O_RDWR | O_TRUNC;

    if(opt->rdir_out_flag || opt->rdir_append_flag) {
        opt->cur_fdout = open(opt->out_path, out_flags, 0666);
        if(opt->cur_fdout == -1) {
            perror("Error opening the output file");
            return -2;
        }
        dup2(opt->cur_fdout, 1);
    }

    return 0;
}

static void itostr(int x, char *dest)
{
    char *tmp;

    for(tmp = dest; tmp != dest + int_buff; tmp++)
        *tmp = '0';
    
    tmp = dest + int_buff;
    *tmp = '\0';
    tmp--;

    do {
        *tmp = x % 10 + '0';    /* + '0' for conversion to ASCII */
        tmp--;
        x /= 10;
    } while(x != 0);
}

void sigchld_handler(int s)
{
    int wr = 0, status;
    char msg_start[] = "\nThe background process with pid = ";
    char msg_int[int_buff];
    char msg_end[] = "  is completed.\n";

    signal(SIGCHLD, sigchld_handler);
    do
    {
        wr = wait4(-1, &status, WNOHANG, 0);
        if(wr != 0 && wr != -1) {
            itostr(wr, msg_int);
            write(1, msg_start, sizeof(msg_start));
            write(1, msg_int, int_buff);
            write(1, msg_end, sizeof(msg_end));
            write(1, "> ", 2);
        }
    } while (wr != 0 && wr != -1);
}