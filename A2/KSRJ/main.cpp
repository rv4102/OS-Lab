#include <signal.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <deque>
#include <map>

#include "commands.h"
#include "pipeline.h"
#include "signal_handlers.h"
#include "utils.h"

using namespace std;

vector<Pipeline*> stored_pipeline; 
map<pid_t, int> pid_to_idx;  
vector <string> history;
pid_t fgpid = 0;
int history_index = 0;
bool up = false;
bool ctrlC = 0, ctrlZ = 0;
struct termios old_tio;
#define GREEN "\033[1;32m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

void setup_terminal(){
    struct termios new_tio;
    tcgetattr(0, &old_tio);     /* grab old terminal i/o settings */
    tcgetattr(0, &new_tio);     /* grab old terminal i/o settings */
    new_tio.c_lflag &= ~(ICANON|ECHO);  /* disable buffered i/o */
    new_tio.c_cc[VMIN] = 1;             /* minimum # of characters to read */
    new_tio.c_cc[VTIME] = 0;            
    tcsetattr(0, TCSANOW, &new_tio);    /* use these new terminal i/o settings now */
}


void print_cmd(string cmd, int cursor_pos, int current_pos, int type){
    int n = cmd.size();
    int up = n;
    if(type == 1) up = n+1; // for backspace and delete
    if(type == 2) up = n-1; // for insert

    for(int i = current_pos; i<up;i++) cout<<"\033[1C";
    for(int i=0; i<up; i++){
        cout<<"\b \b";
    }

    cout<<cmd;
    for(int i=0; i<n-cursor_pos; i++){
        cout<<"\033[1D";
    }
}
void reset_terminal(){
    old_tio.c_lflag |= (ICANON|ECHO);  /* disable buffered i/o */
    old_tio.c_cc[VMIN] = 0;             /* minimum # of characters to read */
    old_tio.c_cc[VTIME] = 1;
    tcsetattr(0, TCSANOW, &old_tio);    /* use these new terminal i/o settings now */
}
void print_prompt(){
    char hostname[1024];
    gethostname(hostname, 1024);
    char cwd[1024];
    getcwd(cwd, 1024);
    cout << GREEN << hostname << RESET << ":" << BLUE << cwd << RESET << "$ ";
}

int main(){
    struct sigaction action;
    action.sa_handler = handle_CTRL;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTSTP, &action, NULL);

    // Reference: https://web.archive.org/web/20170701052127/https://www.usna.edu/Users/cs/aviv/classes/ic221/s16/lab/10/lab.html
    signal(SIGTTOU, SIG_IGN);

    // Reference: https://web.stanford.edu/class/archive/cs/cs110/cs110.1206/lectures/07-races-and-deadlock-slides.pdf
    signal(SIGCHLD, reapProcesses);
    ifstream command_history;
    command_history.open("history.txt");
    if(command_history.is_open()){
        string line;
        while(getline(command_history, line)){
            history.push_back(line);
        }
        command_history.close();
    }      
    history_index = history.size();
    while(1){
        bool tot = false;
        print_prompt();
        string cmd;
        // getline(cin, cmd);
        int cursor_pos = 0;
        while(1){
            // cursor_pos = cmd.size();
            setup_terminal();
            char c = getchar();
            // cout<<(int)c<<" ";
            if(c == 10) {
                break;
            }
            else if(c == 127){//127 --> ascii for backspace
                history_index = history.size();
                if(cursor_pos > 0){
                    cmd.erase(cursor_pos-1, 1);
                    cursor_pos--;
                    print_cmd(cmd, cursor_pos, cursor_pos + 1, 1);
                }
            }
            else if(c == 126){
                // cout<<"delete key"<<endl;
                if(cursor_pos < cmd.size()){
                    cmd.erase(cursor_pos, 1);

                    print_cmd(cmd, cursor_pos,cursor_pos, 1);
                }
            }
            else if(c == 27){//27 --> ascii for escape
                c = getchar();
                c = getchar();
                if(history.empty()) continue;
                if(c=='A'){
                    if(history_index==-1){
                        history_index = history.size();
                    }
                    up = true;
                    //up arrow
                    while(cmd.size() > 0){
                        cmd.pop_back();
                        cout<<"\b \b";
                    }
                    if(history_index >= 0){
                        cout<<history[history_index-1];
                        cmd = history[history_index-1];
                        history_index--;
                    }
                    cursor_pos = cmd.size();
                }
                else if(c=='B'){
                    //down arrow
                    if(history_index == history.size()-1) {
                        history_index = history.size();
                        while(cmd.size() > 0){
                        cmd.pop_back();
                        cout<<"\b \b";
                        }
                    }
                    if(history_index < history.size()-1){
                        while(cmd.size() > 0){
                        cmd.pop_back();
                        cout<<"\b \b";
                    }
                        history_index++;
                        cout<<history[history_index];
                        cmd = history[history_index];
                    
                    }
                    cursor_pos = cmd.size();
                }
                else {
                    history_index = history.size();
                    if(c == 'C'){
                        //right arrow
                        if(cursor_pos < cmd.size()){
                            cout<<"\033[1C";
                            cursor_pos++;
                        }
                    }
                    else if(c == 'D'){
                        //left arrow
                        if(cursor_pos > 0){
                            cout<<"\033[1D";
                            cursor_pos--;
                        }
                    }
                }
            }
            else if(c == 4){
                cout<<"exit"<<endl;
                reset_terminal();
                ofstream save_history;
                save_history.open("history.txt", ios::out | ios::trunc);
                for(int i=0; i<history.size(); i++){
                    save_history << history[i] << endl;
                }
                save_history.close();
                return 0;
            }
            else if(c == 1){
                cursor_pos = 0;
                // cout<<"\033[1D";
                for(int i=0; i<cmd.size(); i++){
                    cout<<"\033[1D";
                }
            }
            else if(c == 5){
                cursor_pos = cmd.size();
                cout<<"\033[1C";
                for(int i=0; i<cmd.size(); i++){
                    cout<<"\033[1C";
                }
            }
            else if (c==-1){
                
                if(ctrlC){
                    cout<<"^C";
                    ctrlC = 0;
                    tot = 1;
                    break;
                }
                else if(ctrlZ){
                    ctrlZ = 0;
                }
                
            }
            else{
                history_index = history.size();
                if(c!=3 && c!=26){  
                    cmd.insert(cursor_pos, 1, c);
                    cursor_pos++;
                    // cmd += c;
                }
                // cout<<c;
                print_cmd(cmd, cursor_pos,cursor_pos-1, 2);
            }
        }
        cout<<endl;
        if(tot) continue;
        // if(cmd == "exit"){
        //     cout<<"EXIT AYA BHAIIII"<<endl;
        //     break;
        // }
        history.push_back(cmd);
        history_index=history.size();
        // trim(cmd);
        cmd = trim(cmd);
        reset_terminal();
        if(cmd.empty()) {
            history.pop_back();
            continue;
        }
        if(cmd == "exit"){
            history.pop_back();
            reset_terminal();
            break;
        }
        Pipeline* p = new Pipeline(cmd);
       
        p->executePipeline(); 
    }
    reset_terminal();
    ofstream save_history;
    save_history.open("history.txt", ios::out | ios::trunc);
    for(int i=0; i<history.size(); i++){
        save_history << history[i] << endl;
    }
    save_history.close();
    return 0;
}
