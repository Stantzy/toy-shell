#ifndef GRPCTRL_H_SENTRY
#define GRPCTRL_H_SENTRY

int change_fg_proc(struct exec_options opt, struct cmd_line cl);
void update_cmd_pgid(int pid, struct exec_options *opt);

#endif