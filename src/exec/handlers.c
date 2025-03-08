#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#include "../../include/exec/exec_structs.h"
#include "../../include/exec/handlers.h"

enum { int_buff = 11 };

volatile sig_atomic_t sigchld_background_flag = 0;

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

int handle_executed_processes(struct exec_options opt, struct cmd_line *cl)
{
	int wr, status;
	int result = 0;

	while(cl != NULL) {
		if(opt.background) {
			printf(
				"New background process with PID=%d created\n",
				cl->pid
			);
		} else {
			while((wr = wait4(cl->pid, &status, 0, NULL)) > 0) {
#ifdef DEBUG         
				if(wr > 0) {
					printf("DEBUG: Process with pid=%d completed ", wr);
					result = handle_wait_result(wr, status);
					printf("with result=%d\n", result);
				}
#endif
            		}
        	}
        	cl = cl->next;
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
		dup2(opt->cur_fdin, STDIN_FILENO);
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
		dup2(opt->cur_fdout, STDOUT_FILENO);
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
	int wr = 0, status, offset = 0;
	char msg_start[] = "The background process with PID=";
	char msg_int[int_buff];
	char *p_tmp = msg_int;
	char msg_end[] = "  is completed.\n";

	signal(SIGCHLD, sigchld_handler);
	do {
		wr = wait4(-1, &status, WNOHANG, 0);
		if(sigchld_background_flag == 1 && wr != 0 && wr != -1) {
			itostr(wr, msg_int);
			while(*p_tmp == '0') {
				p_tmp++;
				offset++;
			}
			write(STDOUT_FILENO, msg_start, sizeof(msg_start));
			write(STDOUT_FILENO, p_tmp, int_buff - offset);
			write(STDOUT_FILENO, msg_end, sizeof(msg_end));
		}
	} while (wr != 0 && wr != -1);
}
