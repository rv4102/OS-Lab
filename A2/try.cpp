#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <termios.h>
#include <glob.h>
#include <signal.h>
// #include <curses.h>

using namespace std;

#define GREEN "\033[1;32m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"
bool parent = 1;
bool ongoing = false;
int run_in_background = 0;
int inotify_fd;
pid_t id_before_execvp;

vector <string> history;
int ind = 0;
bool up = false;
vector<string> temp_files;
string command_executing = "";
void print_prompt(){
    char hostname[1024];
    gethostname(hostname, 1024);
    char cwd[1024];
    getcwd(cwd, 1024);
    cout << GREEN << hostname << RESET << ":" << BLUE << cwd << RESET << "$ ";
}

struct termios old_tio;

void setup_terminal(){
    struct termios new_tio;
    tcgetattr(0, &old_tio);     /* grab old terminal i/o settings */
    tcgetattr(0, &new_tio);     /* grab old terminal i/o settings */
    new_tio.c_lflag &= ~(ICANON|ECHO);  /* disable buffered i/o */
    new_tio.c_cc[VMIN] = 1;             /* minimum # of characters to read */
    new_tio.c_cc[VTIME] = 0;            
    tcsetattr(0, TCSANOW, &new_tio);    /* use these new terminal i/o settings now */
}

void reset_terminal(){
    old_tio.c_lflag |= (ICANON|ECHO);  /* disable buffered i/o */
    old_tio.c_cc[VMIN] = 0;             /* minimum # of characters to read */
    old_tio.c_cc[VTIME] = 1;
    tcsetattr(0, TCSANOW, &old_tio);    /* use these new terminal i/o settings now */
}
string trim(string s){
    int i = 0;
    while(s[i] == ' '){
        i++;
    }
    if(i == s.length()){
        return "";
    }
    s = s.substr(i);
    while(s.back() == ' '){
        s.pop_back();
    }
    return s;
}

vector<string> split(string s, char delim){
    vector<string> v;
    int last = 0;
    for(int i=0; i<s.length(); i++){
        if(s[i] == delim){
            string temp = trim(s.substr(last, i-last));
            if(!temp.empty()){
                v.push_back(temp);
            }
            last = i+1;
        }
    }
    string temp = trim(s.substr(last));
    if(!temp.empty()){
        v.push_back(temp);
    }
    return v;
}

vector<string> handle_input_output_redirection(string cmd){
    bool inp_redir = (cmd.find('<') != string::npos);
    bool out_redir = (cmd.find('>') != string::npos);
    if(!inp_redir && !out_redir){
        return {cmd, "", ""};
    }else if(!inp_redir){
        vector<string> v = split(cmd, '>');
        return {v[0], "", v[1]};
    }else if(!out_redir){
        vector<string> v = split(cmd, '<');
        return {v[0], v[1], ""};
    }else{
        vector<string> v = split(cmd, '<');
        if(v[0].find('>') != string::npos){
            vector<string> v2 = split(v[0], '>');
            return {v2[0], v[1], v2[1]};
        }else{
            vector<string> v2 = split(v[1], '>');
            return {v[0], v2[0], v2[1]};
        }
    }
}

void redirect_in_out(string inp, string out){
    if(!inp.empty()){
        int fd = open(inp.c_str(), O_RDONLY);
        if(fd == -1){
            cout<<"Error: No such file or directory"<<endl;
            exit(1);
        }
        if(dup2(fd, 0) == -1){
            cout<<"Error: Redirection error"<<endl;
            exit(1);
        }
    }
    if(!out.empty()){
        int fd = open(out.c_str(), O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
        if(fd == -1){
            cout<<"Error: No such file or directory"<<endl;
            exit(1);
        }
        if(dup2(fd, 1) == -1){
            cout<<"Error: Redirection error"<<endl;
            exit(1);
        }
    }
}

void signal_handler_CtrlC(int signum){
    cout << endl;
    // if(!ongoing)
    //     print_prompt();
    // // halt the current process
    // fflush(stdout);


}

void signal_handler_CtrlZ(int signum){
    cout<<endl;
    cout << "Shell : " << getpid() << endl;

    /* re-set the signal handler again to catch_int, for next time */
    signal(SIGTSTP, signal_handler_CtrlZ);

    // cout<<"[1]+ Stopped "<<command_executing<<endl;
    // print_prompt();
    // fflush(stdout);
    // if(!run_in_background){
    //     run_in_background = 1;
    // }

    // pid_t curr_proc_pid = getpid();
    // kill(curr_proc_pid, SIGSTOP); // halt(not terminate) the current process
    run_in_background = 0;
    cout << "Yeh chal jao pls : " << id_before_execvp << endl;
    // kill(id_before_execvp, SIGCONT);
}

void execute_command(vector<string> args){
    // expand wildcards
    for(int i=0; i<args.size(); i++){
        if(args[i].find('*') != string::npos || args[i].find('?') != string::npos){ // if globbing is required
            glob_t glob_result; // glob_result is a structure that contains the list of files that match the pattern
            glob(args[i].c_str(), GLOB_TILDE, NULL, &glob_result); // GLOB_TILDE is used to expand ~ to home directory
            for(int j=0; j<glob_result.gl_pathc; j++){ // gl_pathc is the number of files that match the pattern
                args.push_back(glob_result.gl_pathv[j]); // gl_pathv is the list of files that match the pattern
            } 
            args.erase(args.begin()+i); // remove the pattern from the arguments
        }
    }
    char* argv[args.size()+1];
    for(int i=0; i<args.size(); i++){
        argv[i] = (char*)args[i].c_str();
    }
    argv[args.size()] = NULL;
    char* const* argv_dup = (char* const*)argv;
    command_executing= argv[0];
    execvp(argv[0], argv_dup);
    cout<<"Error: Command not found"<<endl;
    exit(1);
}

void process_command(string cmd){
    run_in_background = 0;
    int status = 0;
    if(cmd.back() == '&'){
        run_in_background = 1;
        cmd.back() = ' ';
    }
    bool is_pipe = (cmd.find('|') != string::npos);
    if(is_pipe){
        vector<string> commands = split(cmd, '|');
        int pp[2], pp_old[2];
        for(int i=0; i<commands.size(); i++){
            vector<string> redirections = handle_input_output_redirection(commands[i]);
            if(i!=commands.size()-1){
                pipe(pp);
            }
            pid_t pid = fork();
            if(pid == 0){
                parent = false;
                if(i == 0 || i == commands.size()-1){
                    redirect_in_out(redirections[1], redirections[2]);
                }
                if(i != 0){
                    dup2(pp_old[0], 0);
                    close(pp_old[0]);
                    close(pp_old[1]);
                }
                if(i != commands.size()-1){
                    close(pp[0]);
                    dup2(pp[1], 1);
                    close(pp[1]);
                }
                vector<string> args = split(redirections[0], ' ');
                execute_command(args);
            }
            if(i != 0){
                close(pp_old[0]);
                close(pp_old[1]);
            }
            if(i != commands.size()-1){
                pp_old[0] = pp[0];
                pp_old[1] = pp[1];
            }
            if(!run_in_background){
                waitpid(pid, &status, WUNTRACED);
                if(WIFSTOPPED(status)){
                    kill(pid, SIGCONT);
                }
            }
        }
    }else{
        if(cmd.find("cd") != string::npos){
            vector<string> args = split(cmd, ' ');
            if(args.size() == 1){
                chdir(getenv("HOME"));
            }else{
                for(int i=2; i<args.size(); i++){
                    args[1] += " " + args[i];
                }
                chdir(args[1].c_str());
            }
            return;
        }
        vector<string> redirections = handle_input_output_redirection(cmd);
        vector<string> args = split(redirections[0], ' ');
        pid_t pid = fork();
        if(pid == 0){
            parent = false;
            // signal(SIGTSTP, SIG_DFL);
            redirect_in_out(redirections[1], redirections[2]);
            execute_command(args);
        }
        else{ // in the parent process, get the PID of the child
            signal(SIGINT, SIG_IGN); // the shell should ignore any Ctrl-C press
            signal(SIGTSTP, SIG_IGN); // the shell should ignore any Ctrl-Z press
            if(!run_in_background){
                ongoing = true;
                waitpid(pid, &status, WUNTRACED);
                if(WIFSTOPPED(status)){
                    kill(pid, SIGCONT); // continue the child process
                }
            }
        }  
    }

}
int main(){
    signal(SIGINT, SIG_IGN); // the shell should ignore any Ctrl-C press
    signal(SIGTSTP, SIG_IGN); // the shell should ignore any Ctrl-Z press
    // char firstchar = '\0';
    // char secchar = '\0';
    while(1){
        print_prompt();
        string cmd;
        // getline(cin, cmd);

        while(1){
            setup_terminal();
            char c = getchar();
        // printf("%d\n",c);
            
            // char c = getch();
            if(c == 10) break;//10 --> ascii for newline
            else if(c == 127){//127 --> ascii for backspace
                ind = history.size();
                if(cmd.size() > 0){
                    cmd.pop_back();
                    cout<<"\b \b";
             }  
            }
            // else if(c == 65 && secchar == 91 && firstchar == 27){
            //     //up arrow
            //     cout<<"up arrow pressed";
            // }
            // else if(c== 66 && secchar == 91 && firstchar == 27){
            //     //down arrow
            //     cout<<"down arrow pressed";
            // }
            else if(c == 27){//27 --> ascii for escape
                c = getchar();
                c = getchar();
                if(c=='A'){
                    if(ind==-1){
                        ind = history.size()-1;
                    }
                    up = true;
                    //up arrow
                    // cout<<"up arrow pressed";
                    while(cmd.size() > 0){
                        cmd.pop_back();
                        cout<<"\b \b";
                    }
                    if(ind >= 0){
                        cout<<history[ind-1];
                        cmd = history[ind-1];
                        ind--;
                    }
                    
                }
                else if(c=='B'){
                    //down arrow
                    // cout<<"down arrow pressed";
                    
                    // cout<<ind<<" ";
                    if(ind < history.size()-1){
                        while(cmd.size() > 0){
                        cmd.pop_back();
                        cout<<"\b \b";
                    }
                        ind++;
                        cout<<history[ind];
                        cmd = history[ind];
                    
                    }
                    
                }
                else ind = history.size();
            }
            else{
                ind = history.size();
                cmd += c;
                cout<<c;
            }
        }
        // printf("%")
        cout<<endl;
        if(cmd == "exit"){
            break;
        }
        history.push_back(cmd);
        ind++;
        cmd = trim(cmd);
        reset_terminal();
        if(cmd == "exit"){
            reset_terminal();
            return 0;
        }
        process_command(cmd);
        ongoing = false;
    }
}

