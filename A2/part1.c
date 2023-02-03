#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

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

int main(){
    while(1){
        print_prompt();
        char* cmd = malloc(1000);
        fgets(cmd, 1000, stdin);
        cmd[strlen(cmd)-1] = '\0';
        if(strcmp(cmd, "exit") == 0){
            exit(0);
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
            for(int j = 0; j < i; j++){
                printf("%s ", args[j]);
            }
            printf("\n");
            for(int j=0; j<i; j++){
                if(strcmp(args[j], ">") == 0){
                    freopen(args[j+1], "w", stdout);
                    args[j] = NULL;
                }
                else if(strcmp(args[j], "<") == 0){
                    if(access(args[j+1], F_OK) != -1){
                        freopen(args[j+1], "r", stdin);
                    }
                    else{
                        printf("File not found\n");
                        exit(0);
                    }
                    args[j] = NULL;
                }
            }
            int execvp_return = execvp(args[0], args);
            if(execvp_return == -1){
                printf("Command not found\n");
            }
            exit(0);
        }
        else{
            wait(NULL);
        }
    }

    return 0;
}