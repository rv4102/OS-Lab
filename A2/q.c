
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

void print_prompt(){
    char pcname[100];
    gethostname(pcname, 100);
    char current_dir[200];
    getcwd(current_dir, 200);
    printf("\033[0;32m");
    printf("%s",strcat(pcname, ":"));
    printf("\033[0;34m");
    printf("%s ",strcat(current_dir, "$"));
    printf("\033[0;37m");
}

void run_command(char* args[], int i){
    for(int j=0; j<i; j++){
        if(strcmp(args[j], "<") == 0){
            if(access(args[j+1], F_OK) != -1){
                dup2(open(args[j+1], O_RDONLY), 0);
            }
            else{
                printf("File not found\n");
                exit(0);
            }
            args[j] = NULL;
        }
        else if(strcmp(args[j], ">") == 0){
            dup2(open(args[j+1], O_WRONLY | O_CREAT, 0666), 1);
            args[j] = NULL;
        }
        else if(strcmp(args[j], "|") == 0){
            int fd[2];
            pipe(fd);
            pid_t pid2 = fork();
            if(pid2 == 0){
                close(fd[0]);
                dup2(fd[1], 1);
                close(fd[1]);
                args[j] = NULL;
                run_command(args, j);
            }
            else{
                waitpid(pid2, NULL, 0);
                close(fd[1]);
                dup2(fd[0], 0);
                close(fd[0]);
                run_command(args+j+1, i-j-1);
            }
        }
    }
    int execvp_return = execvp(args[0], args);
    if(execvp_return == -1){
        printf("Command not found\n");
    }
    exit(0);
}
int main(){
    while(1){
        print_prompt();
        char* cmd = malloc(1000);
        fgets(cmd, 1000, stdin);
        cmd[strlen(cmd)-1] = '\0';
        if(strcmp(cmd, "exit") == 0){
            exit(0);
        }
        int run_in_background = 0;
        for(int i=0; i<strlen(cmd); i++){
            if(cmd[i] == '&'){
                run_in_background = 1;
                cmd[i] = ' ';
            }
        }
        pid_t pid = fork();
        if(pid == 0){
            char* args[100];
            int i = 0;
            char* token = strtok(cmd, " ");
            while(token != NULL){
                args[i] = token;
                i++;
                token = strtok(NULL, " ");
            }
            args[i] = NULL;
            // for(int j = 0; j < i; j++){
            //     printf("%s ", args[j]);
            // }
            // printf("\n");
            run_command(args, i);
        }
        else{
            if(run_in_background == 0){
                waitpid(pid, NULL, 0);
            }
        }
    }

    return 0;
}