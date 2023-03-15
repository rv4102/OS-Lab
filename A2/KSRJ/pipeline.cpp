#include "pipeline.h"
#include "signal_handlers.h"

#include <signal.h>
#include <sys/signal.h>
#include <unistd.h>

using namespace std;

Pipeline::Pipeline(string& cmd) : cmd(cmd), is_bg(0), pgid(-1) {}

Pipeline::Pipeline(vector<string>& cmds) : cmds(cmds), is_bg(0), pgid(-1), num_active(cmds.size()), status(RUNNING) {}

void Pipeline::executePipeline() {
    if(cmd.back() == '&'){
        this->is_bg = 1;
        cmd.pop_back();
    }
    if(cmd.find("cd") != string::npos){
        vector<string> args = split(cmd, ' ');
        if(args.size() == 1){
            chdir(getenv("HOME"));
        }
        else{
            for(int i=2; i<args.size(); i++)
                args[1] += " " + args[i];
            chdir(args[1].c_str());
        }
        return;
    }
    this->cmds = split(cmd, '|');
    this->num_active = this->cmds.size();
    pid_t fg_pgid = 0;
    int pp[2], pp_old[2];
    blockSIGCHLD();  
    int cmds_size = this->cmds.size();
    string infile,outfile,command;
    for (int i = 0; i < cmds_size; i++) {
        COMMAND c = handle_input_output_redirection(this->cmds[i]);
        if (i + 1 < cmds_size) {
            int ret = pipe(pp);  
            if (ret < 0) {
                perror("pipe");
            }
        }
        pid_t cpid = fork();
        if (cpid < 0) {
            perror("fork");
            throw "Unable to fork";
        }
        if (cpid == 0) {  
            unblockSIGCHLD();
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            if (i == 0) {
                fg_pgid = getpid();  
            }
            if(i == 0 || i == cmds.size()-1)
                redirect_in_out(c.inp,c.out);
            if (i == 0) {
                setpgrp();
            } else {
                setpgid(0, fg_pgid);  
                dup2(pp_old[0], 0);
                close(pp_old[0]);
                close(pp_old[1]);
            }
            if (i + 1 < cmds_size) {
                 close(pp[0]);
                dup2(pp[1], 1);
                close(pp[1]);
            }
            vector<string> args = split(c.cmd, ' ');
            execute_command(args);
            exit(1);
        
        } else {                       
            if (i == 0) {
                fg_pgid = cpid;
                this->pgid = cpid;
                setpgid(cpid, fg_pgid);         
                stored_pipeline.push_back(this);
                tcsetpgrp(STDIN_FILENO, fg_pgid);
            } else {
                setpgid(cpid, fg_pgid);
            }
            if (i > 0) {
                close(pp_old[0]);
                close(pp_old[1]);
            }
            pp_old[0] = pp[0];
            pp_old[1] = pp[1];

            pid_to_idx[cpid] = (int)stored_pipeline.size() - 1;
        }
    }
    if (this->is_bg) {  
        unblockSIGCHLD();
    } else {
        waitForForegroundProcess(fg_pgid);
        if (stored_pipeline.back()->status == STOPPED) {  
            kill(-fg_pgid, SIGCONT);
        }
    }
    tcsetpgrp(STDIN_FILENO, getpid());
}
