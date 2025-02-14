#ifndef HANDLERS_H_SENTRY
#define HANDLERS_H_SENTRY

extern volatile sig_atomic_t sigchld_background_flag;

int handle_cd_case(char **cmdl);
int handle_executed_process(struct exec_options opt, struct cmd_line *cl);
int handle_redirection(struct exec_options *opt);
void sigchld_handler(int s);

#endif