// https://stackoverflow.com/questions/33064854/ctrlz-signal-handling-in-c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void interpreter() {
    char input[256];
    char dir[PATH_MAX+1];
    char *argv[256];
    int argc = 0;
    char *token;
    if (getcwd(dir, PATH_MAX+1) == NULL) {
        //error occured
        exit(0);
    }
    printf("[shell:%s]$ ", dir);
    fgets(input,256,stdin);

    if (strlen(input) == 0) {
        exit(0);
    }
    input[strlen(input)-1] = 0;
    if (strcmp(input,"") == 0) {
        return;
    }
    token = strtok(input, " ");
    while(token && argc < 255) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    argv[argc] = 0;
    pid_t forknum = fork();
    if (forknum != 0) {
        setpgid(forknum, forknum);
        signal(SIGTTOU, SIG_IGN);
        tcsetpgrp(STDIN_FILENO, forknum);
        tcsetpgrp(STDOUT_FILENO, forknum);
        int status;
        waitpid(forknum, &status, WUNTRACED);
        if(WIFSTOPPED(status)){
            kill(forknum, SIGCONT); // send signal to child to continue
        }

        tcsetpgrp(STDOUT_FILENO, getpid());
        tcsetpgrp(STDIN_FILENO, getpid());
    } else {
        setpgid(0, getpid());
        signal(SIGINT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        setenv("PATH","/bin:/usr/bin:.",1);
        execvp(argv[0], argv);
        if (errno == ENOENT) {
            printf("%s: command not found\n", argv[0]);
        } else {
            printf("%s: unknown error\n", argv[0]);
        }
        exit(0);
    }
}

int main() {
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    while(1) {
        interpreter();
    }
}
