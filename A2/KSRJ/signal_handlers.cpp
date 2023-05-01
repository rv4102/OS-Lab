#include "signal_handlers.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <map>

using namespace std;

void reapProcesses(int signum) {
    int count = 0;
    while (true) {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if (pid <= 0) {
            break;
        }
        
        int id = pid_to_idx[pid];
        Pipeline* pipeline = stored_pipeline[id];
        if (WIFSIGNALED(status) || WIFEXITED(status)) {  
            pipeline->num_active--;
            if (pipeline->num_active == 0) {
                pipeline->status = DONE;
                
            }
        } else if (WIFSTOPPED(status)) {   
            pipeline->num_active--;
            if (pipeline->num_active == 0) {
                pipeline->status = STOPPED;
            }
        } else if (WIFCONTINUED(status)) { 
            pipeline->num_active++;
            if (pipeline->num_active == (int)pipeline->cmds.size()) {
                pipeline->status = RUNNING;
            }
        }
        if (pipeline->pgid == fgpid && !WIFCONTINUED(status)) {
            if (pipeline->num_active == 0) {
                fgpid = 0;  
            }
        }
    }
}

void toggleSIGCHLDBlock(int how) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(how, &mask, NULL);
}

void blockSIGCHLD() {
    toggleSIGCHLDBlock(SIG_BLOCK);
}

void unblockSIGCHLD() {
    toggleSIGCHLDBlock(SIG_UNBLOCK);
}

void waitForForegroundProcess(pid_t pid) {
    fgpid = pid;
    sigset_t empty;
    sigemptyset(&empty);
    while (fgpid == pid) {
        sigsuspend(&empty);
    }
    unblockSIGCHLD();
}

void handle_CTRL(int signum) {
    if (signum == SIGINT) {
        ctrlC = 1;
    } else if (signum == SIGTSTP) {
        ctrlZ = 1;
    }
}

