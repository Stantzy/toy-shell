#ifndef HANDLERS_H_SENTRY
#define HANDLERS_H_SENTRY

int handle_cd_case(char **cmdl);
int handle_executed_process(struct exec_options opt, int pid);
int handle_redirection
    (struct exec_options opt, struct file_descriptors *fd_info);

#endif