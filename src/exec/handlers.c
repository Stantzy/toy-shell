#include "../shared_include/tokens.h"
#include "exec_structs.h"
#include "executor.h"
#include "handlers.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
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
    return opt.redirection_out == 0 && opt.redirection_out_rewrite == 0 &&
            opt.redirection_in == 0;
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

int handle_executed_process(struct exec_options opt, int pid)
{
    int wr, status;
    int result = 0;

    if(opt.background) {
        printf("New background process with pid=%d created\n", pid);
    } else {
        //signal(SIGCHLD, SIG_DFL);
        
        do {
            wr = wait(&status);
        } while(wr != pid);
        
        signal(SIGCHLD, sigchld_handler);
        result = handle_wait_result(wr, status);
    }

    return result;
}

int handle_redirection
(struct exec_options opt, struct file_descriptors *fd_info)
{
    int out_flags = 0;

    if(no_redirections(opt))
        return 0;

    if(opt.redirection_in) {
        fd_info->fd_in = open(opt.in_path, O_RDONLY);
        fd_info->save_stdin = dup(0);
        dup2(fd_info->fd_in, 0);
        if(fd_info->fd_in == -1) {
            perror("Error opening the input file");
            return -1;
        }
    }

    if(opt.redirection_out)
        out_flags = O_CREAT | O_RDWR | O_APPEND;
    else
    if(opt.redirection_out_rewrite)
        out_flags = O_CREAT | O_RDWR | O_TRUNC;

    if(opt.redirection_out || opt.redirection_out_rewrite) {
        fd_info->fd_out = open(opt.out_path, out_flags, 0666);
        if(fd_info->fd_out == -1) {
            perror("Error opening the output file");
            return -2;
        }
        fd_info->save_stdout = dup(1);
        dup2(fd_info->fd_out, 1);
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
        *tmp = x % 10 + '0';    /* + '0' for convert to ASCII */
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