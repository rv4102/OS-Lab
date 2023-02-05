#include<bits/stdc++.h>
#include <termios.h>
#include <dirent.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<signal.h>
#include<sys/inotify.h>

using namespace std;

int bg = 0;  // To check for the background process.
int inotifyd; // The inotify descriptor, this needs to be closed once all the child processes are killed.
vector<string> fileNames; // This will store the file names of all the temporary files created, will be deleted later.
int in_multiWatch=0; // If the current command running is multiWatch or not.

bool prefix(const char * pre, const char * str)
{
    return strncmp(pre,str,strlen(pre)) == 0;
}

struct termios old_tio;
void setup(){
    struct termios new_tio;
    /* get the terminal settings for stdin */
    tcgetattr(STDIN_FILENO, &old_tio);
    /* we want to keep the old setting to restore them a the end */
    tcgetattr(STDIN_FILENO, &new_tio);
    /* disable canonical mode (buffered i/o) and local echo */
    new_tio.c_lflag &= ~(ICANON | ECHO);
    new_tio.c_cc[VMIN] = 1;
    new_tio.c_cc[VTIME] = 0;
    /* set the new settings immediately */
    tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
}
void reset(){
    old_tio.c_lflag |= (ICANON | ECHO);
    old_tio.c_cc[VMIN] = 0;
    old_tio.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO,TCSANOW, &old_tio);
}
// signal handler for ctrl c detection
void signal_handler_CtrlC(int signal_number)
{
    // If multiWatch command is running then we will delete all the files and will close the inotify descriptor.
    if(in_multiWatch)
    {
        close(inotifyd);
        for(int i=0;i<fileNames.size();i++) 
        {
            int t=remove(fileNames[i].data());
        }
    }
}

// Call back function of the ctrl z signal
void signal_handler_CtrlZ(int signal_number)
{
    // If the current command is not a background process then we will set the bg flag to 1.
    if(!bg)
    {
        bg=1;
    }
}

// Least common substring function to get the common substring for the search.
int LCSubStr(string str1, string str2)
{ 
    int N = str1.size();
    int M = str2.size();
    int LCSuff[N + 1][M + 1];
    int mx = 0;    
    for (int i = 0; i <= N; i++)
    {
        for (int j = 0; j <= M; j++)
        {
              if (i == 0 || j == 0){
                LCSuff[i][j] = 0;
              }
 
            else if (str1[i - 1] == str2[j - 1]) {
                LCSuff[i][j] = LCSuff[i - 1][j - 1] + 1;
                if(LCSuff[i][j]>mx)
                {
                    mx = LCSuff[i][j];
                }
            }
            else
                LCSuff[i][j] = 0;
        }
    }
    return mx;
}

// To trim the white spaces from the starting and the ending of string.
string trim(string s)
{
    int i=0;
    while(i<s.size() && s[i]==' ') i++;
    string ret=s.substr(i);
    while(ret.back()==' ') ret.pop_back();
    return ret; 
}

// split the string based on the splitter given, this function is used in various cases to get the flags in the statement
// or getting the different commands in multiWatch etc. The library function can also be used in replacement to this.
vector<string> split(string s, char splitter)
{
    vector<string> ret;
    int last=0;
    int n=s.size();
    for(int i=0;i<n;i++) 
    {
        if(s[i]==splitter)
        {
            string temp=trim(s.substr(last, i-last));
            if(!temp.empty()) ret.push_back(temp);
            last=i+1;
        }
    }
    string temp=trim(s.substr(last));
    if(!temp.empty()) ret.push_back(temp);
    return ret;
}

// This function will execute the command that is sent in form of a string.
void execute_command(vector<string> s)
{
    char * arr[s.size()+1];
    for(int i=0;i<s.size();i++) 
    {
        arr[i] = (char*)(s[i].data());
    }
    arr[s.size()]=NULL;
    char* const* arr1 = arr; // Assign it to a constant array       
    execvp(arr[0], arr1); // Executing the command.
    exit(0);
}

// This function sends the input and output redirection in a statement. Returns strings based on which the io redirection is done later.
pair<string, pair<string, string> > handleInputOutput(string s)
{
    int flag=(s.find('>')!=string::npos);
    int flag1=(s.find('<')!=string::npos);
    // Based on the format and placement of the redirection, file names are obtained.
    if(!flag && !flag1) return {s, {"", ""}};
    else if(!flag)
    {
        vector<string> in = split(s, '<');
        return {in[0], {in[1], ""}};
    }
    else if(!flag1)
    {
        vector<string> out = split(s, '>');
        return {out[0], {"", out[1]}};
    }
    else 
    {
        vector<string> in = split(s, '<');
        if(in[0].find('>')!=string::npos)
        {
            vector<string> out = split(in[0], '>');
            return {out[0], {in[1], out[1]}};
        }
        else 
        {
            vector<string> redirect = split(in[1], '>');
            return {in[0], {redirect[0], redirect[1]}};
        }
    }
}

// The input output directions are done and handled with this function
void handleRedirection(pair<string, string> direction)
{
    // If there is an input redirections, then changing the standard input to the file descriptor.
    if(!(direction.first).empty())
    {
        int in = open(direction.first.c_str(), O_RDONLY);
        if(in<0)
        {
            cout<<"Unable to open file for input. Try again"<<endl;
            exit(EXIT_FAILURE);
        }
        if(dup2(in, 0)<0)
        {
            cout<<"Error while redirecting input. Try again."<<endl;
            exit(EXIT_FAILURE);
        }
    }
    // If there is an output redirections, then changing the standard output to the file descriptor.
    if(!(direction.second).empty())
    {
        int out = open(direction.second.c_str(), O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
        if(out<0)
        {
            cout<<"Unable to open file for output. Try again"<<endl;
            exit(EXIT_FAILURE);
        }
        if(dup2(out, 1)<0)
        {
            cout<<"Error while redirecting output. Try again."<<endl;
            exit(EXIT_FAILURE);
        }
    }
}

// This functions processes the string for different methods as explained in the assignment. The background and the pipe commands are handled. 
void processInputCommand(string s)
{
    bg=0;
    int  status = 0;
    if(s.back() == '&')
    {
        bg = 1;
        s.back() = ' ';
    }
    int flag = (s.find('|')!=string::npos);
    if(flag)
    {
        vector<string> commands = split(s, '|');
        int pp[2], oldpp[2];
        for(int i=0;i<commands.size();i++) 
        {
            pair<string, pair<string, string> > redirections=handleInputOutput(commands[i]);
            if(i!=commands.size()-1) pipe(pp);
            int x=fork();
            // Piping the commands with input output redirections.
            if(!x)
            {
                if(i==0 || i==commands.size()-1) handleRedirection(redirections.second);// To handle the input/output in the last or first statement.
                if(i!=0)
                {
                    dup2(oldpp[0], 0);
                    close(oldpp[0]);
                    close(oldpp[1]);
                }
                if(i!=commands.size()-1)
                {
                    close(pp[0]);
                    dup2(pp[1], 1);
                    close(pp[1]);
                }
                vector<string> temp=split(redirections.first, ' ');
                execute_command(temp);
            }
            if(i!=0)
            {
                close(oldpp[0]);
                close(oldpp[1]);
            }
            if(i!=commands.size()-1) 
            {
                oldpp[0]=pp[0];
                oldpp[1]=pp[1];
            }
            if(!bg) 
            {
                waitpid(x,&status,WUNTRACED);
                if(WIFSTOPPED(status))
                {
                    kill(x,SIGCONT);
                }
            }
        }
    }
    else  // If there exist only a single command and no piping is done.
    {
        pair<string, pair<string, string> > redirections=handleInputOutput(s);
        vector<string> temp=split(redirections.first, ' ');
        pid_t x = fork();
        if(x == 0)
        {
            handleRedirection(redirections.second);
            execute_command(temp);
        }
        if(!bg) 
        {
            waitpid(x,&status,WUNTRACED);
            if(WIFSTOPPED(status))
            {
                kill(x,SIGCONT);
            }
        }
    }
}

// This functions handles the child count while the multiWatch command is running. And also while any child process ends.
int child_count=0;
void decrease_child(int signal)
{
    child_count--;
    if(child_count==0)
    {
        close(inotifyd);
        for(int i=0;i<fileNames.size();i++) 
        {
            int t=remove(fileNames[i].data());
        }
    }
}

// This function handles the process related to multiWatch command. Here we keep a watch on file changes with the help
// of inotify library. Separate child processes are created with the help of fork command, and file descriptor for these are stored in order
// to get the output from each command as asked in the assignment. From this the output is printed in the required format.
void handleMultiWatch(string s)
{
    vector<string> commands = split(s.substr(10), '"');
    inotifyd = inotify_init();
    vector<int> readDescriptor;
    fileNames.clear();
    map<int, int> watch2File;
    child_count+=commands.size();
    signal (SIGCHLD, decrease_child);
    for(int i=0;i<commands.size();i++) 
    {
        int temp = fork();
        if(temp==0)
        {
            auto pid = getpid();
            string fin = commands[i] + " > .temp." + to_string(pid) + ".txt";
            processInputCommand(fin);
            exit(0);
        }
        else 
        {
            string fileName = ".temp." + to_string(temp) + ".txt"; // File name according to the format given.
            fileNames.push_back(fileName); // Storing the file name.
            readDescriptor.push_back(open(fileName.c_str(), O_CREAT | O_RDONLY, 0666)); // opening the file for reading.
            int desc = inotify_add_watch(inotifyd, fileName.c_str(), IN_MODIFY); // watching the file for any changes.
            watch2File[desc] = i;
        }
    }
    while(true)
    {
        char buffer[4096]__attribute__ ((aligned(__alignof__(struct inotify_event))));
        if(child_count==0) 
        {
            close(inotifyd);
            for(int i=0;i<fileNames.size();i++) {
                int t=remove(fileNames[i].data());
            }
            break;
        }
        int length = read(inotifyd, buffer, sizeof(buffer));
        if(length<=0 || child_count==0) break;
        int i=0;
        // Getting the outputs in the parent function
        while(i<length)
        {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            string output="";
            int index=-1;
            if(event->mask & IN_MODIFY)
            {
                int watchDesc = event->wd;
                index = watch2File[watchDesc];
                char read_buffer[4096];
                while(read(readDescriptor[index], read_buffer, 4095)>0){
                    output+=string(read_buffer);
                    bzero(&read_buffer, sizeof(read_buffer));
                }
            }
            if(!output.empty() && index!=-1)
            {
                cout<<commands[index]<<", "<<time(NULL)<<" : "<<endl;
                cout<<"<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-"<<endl;
                cout<<output;
                cout<<"->->->->->->->->->->->->->->->->->->->"<<endl<<endl;
            }
            i += sizeof(struct inotify_event) + event->len;
        }
    }
}

int main()
{
    signal(SIGINT, signal_handler_CtrlC);
    signal(SIGTSTP, signal_handler_CtrlZ);
    vector<string> cmds;
    ifstream fin; 
    string input;
    fin.open("history.txt");
    while(getline (fin, input))
    {
        cmds.push_back(input);
    }
    reverse(cmds.begin(),cmds.end());
    int  status = 0;
    while(1)
    {
        in_multiWatch=0;
        cout<<">>> ";
        string s;
        setup();
        // tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
        int cont = 0;
        while(1) {
            char c = getchar();
            if(int (c) == 10) break; // This is the ascii code for linefeed
            else if(int(c) == 127){ // This is the ascii value for backspace   
                if(!s.empty()){
                fputs("\b \b",stdout);
                s.pop_back();
                }
                  
            }
            else if(int(c) == 9) // This is the ascii code for tab press
            {
                string check;
                int j = s.size()-1;
                while(j>=0 && s[j] != ' ' && s[j] != '/'){
                    check.push_back(s[j]);
                    j--;
                }
                reverse(check.begin(),check.end());
                char pre[check.size()+1];
                strcpy(pre, check.c_str());
                DIR *dir; struct dirent *diread;
                vector<char *> match;
                
                if ((dir = opendir("./")) != nullptr) {
                    while ((diread = readdir(dir)) != nullptr) {
                        if(prefix(pre,diread->d_name)) match.push_back(diread->d_name);
                    }
                    closedir (dir);
                } else {
                    perror ("opendir");
                    return EXIT_FAILURE;
                }
                if(match.size() == 1)
                {
                    j = check.size();
                    string matched = match[0];
                    while(j<matched.size())
                    {
                        cout<<matched[j];
                        s.push_back(matched[j]);
                        j++;
                    }
                }
                else if(match.size()>1)
                {
                    int k = check.size();
                    int flag1 = 0;
                    char ch;
                    while(!flag1)
                    {
                        for (int i = 0; i < match.size(); i++)
                        {
                            if(k==strlen(match[i]))
                            {
                                flag1 = 1;
                                break;
                            }
                            if(i == 0) ch = match[i][k];
                            else{
                                if(ch != match[i][k])
                                {
                                    flag1 = 1;
                                    break;
                                }
                            }
                        }
                        if(flag1) break;
                        else cout<<ch;
                        k++;
                    }
                    cout<<"\n";
                    for (int i = 0; i < match.size(); i++)
                    {
                        cout<<i+1<<") "<<match[i]<<"\n";
                    }
                    int a = 1;
                    reset();
                    do{
                        cout<<"Enter number corresponding to the required string: "<<endl;
                        cin>>a;
                    }
                    while(a<=0 || a>match.size());
                    setup();
                    c = getchar();
                    cout<<">>> ";
                    cout<<s;
                    j = check.size();
                    string matched = match[a-1];
                    while(j<matched.size())
                    {
                        cout<<matched[j];
                        s.push_back(matched[j]);
                        j++;
                    }
                    c = char(9);
                }
            }
            else if(int(c) == 18) // This is the ascii code for ctrl r
            {
                cout<<"Enter search term"<<endl;
                s.clear();
                char p;
                while(p = getchar())
                {
                    if(int(p) == 10) break;
                    if(int(p) == 127 && !s.empty())
                    {
                        cout<<"\b \b";
                        s.pop_back();
                        continue;
                    }
                    cout<<p;
                    s.push_back(p);
                }
                cout<<"\n";
                auto j = find(cmds.begin(), cmds.begin()+min((int)cmds.size(),10000),s);
                if(j != cmds.begin()+min((int)cmds.size(),10000)) cout<<*j<<"\n";
                else{  
                    string ans = "";
                    int m = 0;
                    for (int i = 0; i < min((int)cmds.size(),10000); i++)
                    {
                        int temp = LCSubStr(s, cmds[i]);
                        if(temp>m && temp > 2){
                            ans = cmds[i];
                            m = temp;
                        } 
                    }
                    if(ans.size()>2) cout<<ans<<"\n";
                    else cout<<"No match for search term in history"<<"\n";
                }
                cont = 1;
                break;
            }
            if(int(c) != 9 && int(c) != 127){
                cout<<c;
                s.push_back(c);
            }
        }
        if(cont == 1) continue;
        cout<<"\n";
        cmds.push_back(s);
        s=trim(s);
        reset();
        // If history command is called, printing the last 1000 or the complete set of commands.
        if(s.compare("history") == 0)
        {
            for (int i = 0; i < min(1000,int(cmds.size())); i++)
            {
                cout<<cmds[cmds.size()-1-i]<<"\n";
            }
        }
        // Exit the code and store the commands used in history.txt .
        if(s.compare("exit") == 0)
        {
            reverse(cmds.begin(),cmds.end());
            ofstream fout("history.txt");
            for (int i = 0; i<min(10000,(int)cmds.size()); i++)
            {
                fout<<cmds[i]<<"\n";
            }
            reset();
            return 0;
        }
        // If multiwatch then call the appropriate function and continue with the command.
        if(s.substr(0, 10)=="multiWatch") 
        {
            in_multiWatch=1;
            handleMultiWatch(s);
            continue;
        } 
        processInputCommand(s);
    }
    return 0;
}
