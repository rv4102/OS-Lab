#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <ctype.h>
#include<time.h>

/* reads from keypress, doesn't echo */
int getch(void){
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

int background = 0;
pid_t pp = 0;
void sigHandler(int signum){
    switch(signum){
        case SIGTSTP:
            printf("Moved to background\n");
            fflush(stdout);
            kill(pp, SIGUSR1);
            break;

        case SIGINT:
            printf("Interrupted\n");
            fflush(stdout);
            break;
    }
}


void autocomplete(char* buf){
    char *str = strdup(buf);
    char *rem;
    for (int j = 0; ; j++, str = NULL) {
        char* token = strtok(str, " ");
        if (token == NULL)
            break;
        rem = token;
    }
    free(str);
    // printf("\n%s\n",rem);

    char direc[100];
    struct dirent* d;
    DIR *dh = opendir(getcwd(direc,100));
    char *files[100];
    char *sf[100];
    int cnt = 0;

    int i=0;
    while((d=readdir(dh))!=NULL){
        printf("%s\n",d->d_name);
        if(d->d_name[0]=='.') continue;
        
        if(strncmp(d->d_name, rem, strlen(rem))==0){
            files[cnt] = strdup(d->d_name);
            sf[cnt++] = strdup(files[cnt]+strlen((rem)));
        }
    }
    if(cnt==0){
        return;
    }
    else if(cnt==1){
        strcat(buf, sf[0]);
        printf("%s",sf[0]);
        return;
    }
    else{
        printf("\n");
        for(int i=0;i<cnt;i++){
            printf("%d. %s\t",i+1,files[i]);
        }
        printf("\n");
        char c = getch();
        write(STDOUT_FILENO, "\033[F", 5);
        write(STDOUT_FILENO, "\033[K", 5);
        printf("\b \b");
        write(STDOUT_FILENO, "\033[F", 5);
        write(STDOUT_FILENO, "\033[K", 5);
        int x = c - '1';
        strcat(buf, sf[x]);
        printf("myShell$> %s",buf);
        return; 
    }
}


void MultiWatch(int numComms, char* comms[]){
    int epoll_fd = epoll_create1(0);

    if(epoll_fd == -1){
        perror("epoll_create1");
    }

    struct epoll_event event[numComms], outEvent[numComms];

    int fds[100][2]; 

    pid_t pid;

    struct Pair{
        int fd;
        char* command;
    };

    struct Pair mp[500];
    int numP = 0;

    for(int i=0;i<numComms;i++){
        if(pipe(fds[i]) == -1){
            perror("pipe");
            continue;
        }
        mp[numP].fd = fds[i][0];
        mp[numP].command = comms[i];
        numP++;
        if((pid = fork()) < 0) {
            perror("fork");
        } 
        else if(pid == 0) {     
            dup2(fds[i][1], STDOUT_FILENO);
            close(fds[i][1]);

            int args_num = 0;
            char* args[10];
            char *token, *str1;
            str1 = strdup(comms[i]);

            for (int j = 0; ; j++, str1 = NULL) {
                token = strtok(str1, " ");
                if (token == NULL)
                    break;
                
                args[j] = token;
                args_num += 1;
            }
            free(str1);
            args[args_num] = (char *) NULL;            

            if(execvp(args[0],args)==-1){
                perror("execvp");
            }
            exit(0);
        }
        event[i].events = EPOLLIN;
        event[i].data.fd = fds[i][0];
        
        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fds[i][0], &(event[i]))){
            perror("epoll_ctl");
        }
        
    }
    time_t t;   // not a primitive datatype
    
    while(1){
        int event_count = epoll_wait(epoll_fd, outEvent, 100, 50000);
        time(&t);
        if(event_count==0 || event_count==-1){
            break;
        }
        for(int i=0;i<event_count;i++){
            int tempFd = outEvent[i].data.fd;
            char* command;
            for(int i=0;i<numP;i++){
                if(mp[i].fd == tempFd){
                    command = strdup(mp[i].command);
                    break;
                }
            }
            printf("%s: %s",command, ctime(&t));
            printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
            char read_buffer[1000];
            read(outEvent[i].data.fd, read_buffer, 1000);
            printf("%s\n", read_buffer);
            printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
        }
    }


    return;   
}

enum {MAXLINE = 256, MAXARGS = 48};

////////////////////////////////////////////////////////////////////////////////////////
struct Node{
    char* command;
    struct Node* next;
    struct Node* prev;
};

void InsertInEnd(struct Node** headAddr, struct Node** tailAddr, char* inp){
    struct Node* head = *headAddr;
    struct Node* tail = *tailAddr;
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->command = strdup(inp);
    newNode->next = NULL;
    newNode->prev = *tailAddr;
    if(*tailAddr == NULL){
        *headAddr = newNode;
        *tailAddr = newNode;
    }
    else{
        (*tailAddr)->next = newNode;
        *tailAddr = newNode;
    }
    return;
}

void DeleteFromBeg(struct Node** headAddr){
    struct Node* nHead = (*headAddr)->next;
    nHead->prev = NULL;
    *headAddr = nHead;
    return;
}

int LongestMatch(char* text, char* query){
    int len = strlen(text) + strlen(query) + 1; 
    char inp[len+1];
    inp[len] = '\0';
    int j = 0;
    for(int i=0;query[i]!='\0';i++){
        inp[j] = query[i];
        j++;
    }
    inp[j++] = '#';
    for(int i=0;text[i]!='\0';i++){
        inp[j] = text[i];
        j++;
    }

    int lps[len];
    lps[0] = 0;
    for(int i=1;i<len;i++){
        int j = lps[i-1];
        while(j>0 && inp[i]!=inp[j]){
            j = lps[j-1];
        }
        if(inp[i]==inp[j]){
            j++;
        }
        lps[i] = j;
    }
    
    int maxMatch = 0;
    for(int i=0;i<len;i++){
        if(lps[i] > maxMatch){
            maxMatch = lps[i];
        }
    }
    return maxMatch;
}

void searchHistory(struct Node *tail, char* buf){
    printf("Enter command to be searched:- ");
    char inp[1000];
    scanf("%[^\n]s", inp);
    int maxMatch = 0;
    struct Node* matchPt = NULL;
    struct Node* temp = tail;
    while(temp!=NULL){
        int k = LongestMatch(temp->command, inp);
        if(maxMatch < k && (strlen(inp)<=2 || k>2)){
            maxMatch = k;
            matchPt = temp;
        }
        temp = temp->prev;
    }
    if(matchPt==NULL){
        printf("No match found\n");
    }
    else{
        printf("Matched - %s", matchPt->command);
        printf("Do yo want to execute it? [y or n] ");
        char resp;
        getchar();
        resp = getchar();
        if(resp == 'y'){
            strcpy(buf, matchPt->command);
            return;
        }
    }
    strcpy(buf, strdup("\n"));
    return;
}
////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    struct Node* head = NULL;
    struct Node* tail = NULL;
    int numCommands = 0;

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read1;
    
    fp = fopen("history.txt", "a");
    fclose(fp);

    fp = fopen("history.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read1 = getline(&line, &len, fp)) != -1) {
        InsertInEnd(&head, &tail, line);
        numCommands++;
    }
    fclose(fp);

    char buf[MAXLINE];
    pid_t pid, wpid;
    int status;

    char *str1, *str2, *token;
    printf("myShell$> ");  

    signal(SIGINT, sigHandler);
    signal(SIGTSTP, sigHandler);

    while (1) {
        if(numCommands == 10000){
            DeleteFromBeg(&head);
            numCommands--;
        }
        for(int i=0;i<MAXLINE;i++){
            buf[i] = '\0';
        }
        char c;
        int i=0;        
        do{
            c = getch();
            if(c == CTRL('r')){
                searchHistory(tail, buf);
                i = strlen(buf);
            }
            if(c=='\t'){
                buf[i] = '\0';
                autocomplete(buf);
                i = strlen(buf);
            }
            else if(c==127){
                printf("\b \b");
                i--;
            }
            else{
                buf[i++] = c;
                printf("%c",c);
            }
        } while (c!='\n' && c!='\r');

        buf[i] = '\0';   
     

        if(buf[0] !='\n'){
            for(int i=0;i<strlen(buf);i++){
                if(buf[i]==18){
                    buf[i] = '\0';
                }
            }
            InsertInEnd(&head, &tail, buf);numCommands++;
        }

        char *args[MAXARGS];
        int args_num = 0;
        int in = 0, out = 0;
        char infile[50], outfile[50];
        background = 0;

        if(strcmp(buf, "exit\n")==0 || strcmp(buf, "quit\n")==0){
            FILE *fOut;
            fOut = fopen("history.txt", "wb");
            while(head != NULL){
                fputs(head->command, fOut);
                head = head->next;
            }
            fclose(fOut);
            exit(0);
        }

        if(strcmp(buf, "clear\n")==0){
            char* CLEAR_SCREEN_ANSI = " \e[1;1H\e[2J";
            write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
            printf("myShell$> ");
            continue;
        }

        if(strcmp(buf, "history\n")==0){
            int i=0;
            struct Node* temp= head;
            while(temp!=NULL){
                i++;
                printf("%d\t%s\n", i, temp->command);
                temp = temp->next;
            }
            printf("myShell$> ");
            continue;
        }

        i = strlen(buf)-1;
        while(buf[i]==18 || buf[i]=='\n'){
            buf[i--] = '\0';
        }

        if(strncmp(buf, "multiWatch", (size_t)10) == 0){
            int numComms = 0;
            char *comms[100];
            char *str;
            str = strdup(buf+11);
            

            for (int j = 0; ; j++, str = NULL) {
                token = strtok(str, ",");
                if (token == NULL)
                    break;
                
                comms[j] = token;
                numComms += 1;
                // printf("%d: %s\n", j + 1, args[j]);
            }

            MultiWatch(numComms, comms);
            printf("myShell$> ");
            continue;
        }

        background = 0;
        int amp = 0;
        for(;buf[amp]!=0;amp++){
            if(buf[amp]=='&'){
                background = 1;
                break;
            }
        }
        if(background){
            str1 = strndup(buf, amp);
            str2 = strndup(buf, amp);
        }
        
        // redirection of output
        for(int i=0;i<strlen(buf);i++){
            if(buf[i]=='>'){
                out = i;
                break;
            }
        }
        if(out){
            int i = out;
            i++;
            while(buf[i]==' ') i++;
            int k = i;
            while(buf[i]!=' ' && buf[i]!=0){
                outfile[i-k] = buf[i];
                i++;
            }
            outfile[i-k] = '\0';
            // printf("%s\n", outfile);
            str1 = strndup(buf,out);
            str2 = strndup(buf,out);
        }
        
        // redirection of input
        for(int i=0;i<strlen(buf);i++){
            if(buf[i]=='<'){
                in = i;
                break;
            }
        }
        if(in){
            int i = in;
            i++;
            while(buf[i]==' ') i++;
            int k = i;
            while(buf[i]!=' ' && buf[i]!=0 && buf[i]!='>'){
                infile[i-k] = buf[i];
                i++;
            }
            infile[i-k] = '\0';
            // printf("%s\n", infile);
            str1 = strndup(buf,in);
            str2 = strndup(buf,in);
        }
        
        if(!in && !out && !background){
            str1 = strdup(buf);
            str2 = strdup(buf);
        }

        char *comms[100];
        int NumComms = 0;
        for (int j = 0; ; j++, str2 = NULL) {
            token = strtok(str2, "|");
            if (token == NULL)
                break;
            
            comms[j] = token;
            NumComms += 1;
            // printf("%d: %s\n", j + 1, args[j]);
        }

        // int saved_in = dup(0);
        // int saved_out = dup(1);

        if(NumComms > 1){
            char* token;
            int pipefds[2];
            if(pipe(pipefds) == -1){
                perror("pipe");
                continue;
            }
            char c[100];
            for(int i=0;i<NumComms;i++){
                printf("NumComms = %d, Current = %d\n", NumComms, i);
                if((pid = fork()) < 0) {
                    perror("fork");
                } 
                else if(pid == 0){
                    char* args[10];
                    int args_num = 0;
                    char* str3 = strdup(comms[i]);
                    for (int j = 0; ; j++) {
                        token = strtok(str3, " ");
                        if (token == NULL)
                            break;
                        
                        args[j] = token;
                        args_num += 1;
                        str3 = NULL;
                    }
                    args[args_num] = (char *) NULL;
                    
                    if(i==0){
                        dup2(pipefds[1], STDOUT_FILENO);
                        close(pipefds[1]);
                        if(in){
                            int fd0 = open(infile, O_RDONLY);
                            dup2(fd0, STDIN_FILENO);
                            close(fd0);
                        }
                    }
                    else if(i==NumComms-1){
                        dup2(pipefds[0], STDIN_FILENO);
                        close(pipefds[0]);
                        if(out){
                            int fd1 = creat(outfile , 0644);
                            dup2(fd1, STDOUT_FILENO);
                            close(fd1);
                        }
                    }
                    else{
                        dup2(pipefds[0], STDIN_FILENO);
                        close(pipefds[0]);
                        if(pipe(pipefds) == -1){
                            perror("pipe");
                            continue;
                        }
                        dup2(pipefds[1], STDOUT_FILENO);
                        close(pipefds[1]);
                    }

                    if(execvp(args[0],args)==-1){
                        perror("execvp");
                    }
                    exit(0);
                }
                close(pipefds[1]);
                if(waitpid(pid, &status, 0) < 0){
                    perror("waitpid");
                }
            }
            printf("myShell$> ");
            continue;
        }
        
        for (int j = 0; ; j++, str1 = NULL) {
            token = strtok(str1, " ");
            if (token == NULL)
                break;
            
            args[j] = token;
            args_num += 1;
        }
        free(str1);
        args[args_num] = (char *) NULL;
        
        if((pid = fork()) < 0) {
            perror("fork");
        } 
        else if(pid == 0) {
            if(background && !out){
                int fd = creat(".background.proc", 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
                remove(".background.proc");
            }
            if(in){
                int fd0 = open(infile, O_RDONLY);
                dup2(fd0, STDIN_FILENO);
                close(fd0);
            }
            if(out){
                int fd1 = creat(outfile , 0644) ;
                dup2(fd1, STDOUT_FILENO);
                close(fd1);
            }
            execvp(args[0], &args[0]);
            // printf("couldn't execute: %s\n", buf);
            perror("execvp");
            exit(0);
        }

        if(background){
            printf("myShell$> ");
            continue;
        }
        
        if(waitpid(pid, &status, 0) < 0){
            perror("waitpid");
        }
        printf("myShell$> ");
    }

    exit(EXIT_SUCCESS);
}