#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#include "../../include/exec/exec_structs.h"
#include "../../include/exec/grpctrl.h"

int change_fg_proc(struct exec_options opt, struct cmd_line cl)
{
    int res;
    
    if(opt.background == 0 && opt.cmd_pgid != 0) {
        signal(SIGTTOU, SIG_IGN);
        res = tcsetpgrp(STDIN_FILENO, opt.cmd_pgid);
    }

    return res;
}

void update_cmd_pgid(int pid, struct exec_options *opt)
{
    if(pid > 0 && opt->cmd_pgid == 0)
        opt->cmd_pgid = pid;
}
