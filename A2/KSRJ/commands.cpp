#ifndef COMMANDS_H
#define COMMANDS_H
#include "commands.h"
#include "utils.h"

pid_t get_parent(pid_t pid){
    std::ostringstream stat_path;
    stat_path << "/proc/" << pid << "/stat";
    std::ifstream stat_file(stat_path.str());
    if (!stat_file.is_open()) {
        perror("Failed to open stat file");
        return 1;
    }
    std::string line;
    std::getline(stat_file, line);
    stat_file.close();

    std::istringstream line_stream(line);
    std::string dummy;
    int ppid;
    for (int i = 0; i < 3; ++i) {
        line_stream >> dummy;
    }
    line_stream >> ppid;
    return ppid;
}
double get_cpu_usage(pid_t pid)
{
    std::string stat_file = "/proc/" + std::to_string(pid) + "/stat";
    std::string stat_contents;
    std::ifstream stream(stat_file);
    std::getline(stream, stat_contents);
    stream.close();

    // Split the contents of the stat file by spaces
    std::vector<std::string> values;
    std::stringstream stat_stream(stat_contents);
    std::string buffer;
    while (stat_stream >> buffer)
    {
        values.push_back(buffer);
    }

    // Get the values of utime and stime
    long utime = std::stol(values[13]);
    long stime = std::stol(values[14]);
    long starttime = std::stol(values[21]);

    // Get the system uptime
    std::string uptime_file = "/proc/uptime";
    std::string uptime_contents;
    stream = std::ifstream(uptime_file);
    std::getline(stream, uptime_contents);
    stream.close();

    // Split the contents of the uptime file by spaces
    values.clear();
    std::stringstream uptime_stream(uptime_contents);
    while (uptime_stream >> buffer)
    {
        values.push_back(buffer);
    }

    // Get the value of uptime
    long uptime = std::stol(values[0]);

    // Calculate the total time the process has spent in user mode and kernel mode
    long total_time = utime + stime;

    // Calculate the seconds since the process started
    double seconds_since_start = uptime - (starttime / sysconf(_SC_CLK_TCK));

    // Calculate the current CPU usage of the process
    return ((total_time / sysconf(_SC_CLK_TCK)) / seconds_since_start) * 100;
}

pid_t detect_malware(vector<pid_t> pids){
    int size = pids.size();
    for(int i = 1; i < size; i++){
        if(get_cpu_usage(pids[i]) / get_cpu_usage(pids[i-1]) < 0.001){
            return pids[i];
        }
    }
    return 0;
}
vector<pid_t> get_parents(pid_t pid){
    vector<pid_t> parents;
    while(pid != 0){
        parents.push_back(pid);
        pid = get_parent(pid);
    }
    return parents;
}

COMMAND handle_input_output_redirection(string cmd){
    bool inp_redir = (cmd.find('<') != string::npos);
    bool out_redir = (cmd.find('>') != string::npos);
    vector<string> ans;
    if(!inp_redir && !out_redir){
        ans = {cmd, "", ""};
        
    }else if(!inp_redir){
        vector<string> v = split(cmd, '>');
        ans = {v[0], "", v[1]};
    }else if(!out_redir){
        vector<string> v = split(cmd, '<');
        ans = {v[0], v[1], ""};
    }else{
        vector<string> v = split(cmd, '<');
        if(v[0].find('>') != string::npos){
            vector<string> v2 = split(v[0], '>');
            ans = {v2[0], v[1], v2[1]};
        }else{
            vector<string> v2 = split(v[1], '>');
            ans = {v[0], v2[0], v2[1]};
        }
    }
    return COMMAND(ans);
}
int delep(string filename)
{
    int fd[2];
    pipe(fd);
    pid_t process = fork();
    if (process == 0)
    {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        DIR *dir;
        struct dirent *entry;
        if ((dir = opendir("/proc")) == NULL)
        {
            std::cerr << "Error opening /proc directory: " << strerror(errno) << std::endl;
            return 1;
        }
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type != DT_DIR)
            {
                continue;
            }
            int pid = -1;
            try
            {
                pid = std::stoi(entry->d_name);
            }
            catch (const std::invalid_argument &e)
            {
                continue;
            }
            std::string fd_path = "/proc/" + std::to_string(pid) + "/fd";
            DIR *fd_dir;
            if ((fd_dir = opendir(fd_path.c_str())) == NULL)
            {
                continue;
            }
            // bool has_file_open = false;
            struct dirent *fd_entry;
            while ((fd_entry = readdir(fd_dir)) != NULL)
            {
                std::string link_path = fd_path + "/" + fd_entry->d_name;
                char buffer[1024];
                ssize_t len = readlink(link_path.c_str(), buffer, 1024 - 1);
                if (len == -1)
                {
                    continue;
                }
                buffer[len] = '\0';
                if (strstr(buffer, filename.c_str()) != NULL)
                {
                    cout << pid << " ";
                    break;
                }
            }
            closedir(fd_dir);
        }
        closedir(dir);
        exit(0);
    }
    close(fd[1]);
    string out;
    char c;
    while (read(fd[0], &c, 1) > 0)
        out += c;
    close(fd[0]);
    vector<string> out_sep = split(out, ' ');
    vector<int> pids;
    for (auto pid_str : out_sep)
        pids.push_back(stoi(pid_str));
    cout << "Processes with file open: " << endl;
    for (int i = 0; i < pids.size(); i++)
    {
        cout << pids[i] << " ";
    }
    cout << endl;

    cout << "Kill processes? (y/n): ";
    char choice[100];
    fgets(choice, 100, stdin);
    if (choice[0] == 'y')
    {
        for (int i = 0; i < pids.size(); i++)
        {
            kill(pids[i], SIGKILL);
        }
    }
    else if (choice[0] == 'n')
    {
        return 0;
    }
    else
    {
        cout << "Invalid choice" << endl;
        return 1;
    }

    return 0;
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

void execute_command(vector<string> args){
    if(args.empty()){
        return;
    }
    if(args[0] == "sb"){
        int status;
        int argc = args.size();
        if(argc == 1){
            cout<<"Usage: sb <pid> -suggest"<<endl;
        }else if(argc <= 3){
            pid_t create_proc = fork();
            if(create_proc == 0){
                pid_t pid = stoi(args[1]);
                vector<pid_t> parents = get_parents(pid);
                cout<<"Parent processes: ";
                for(auto p: parents){
                    cout<<p<<" ";
                }cout<<endl;
                if(argc == 3 && args[2] == "-suggest"){
                    pid_t malware_pid = detect_malware(parents);
                    if(malware_pid != 0){
                        cout << "Malware detected with pid : " << malware_pid << endl;
                    }
                    else{
                        cout << "No malware detected" << endl;
                    }
                }
                exit(0);
            }else{
                waitpid(create_proc, &status, WUNTRACED);
                if(WIFSTOPPED(status)){
                    kill(create_proc, SIGCONT);
                }
            }
        }
        return;
    }
    else if(args[0] == "delep"){
        if(args.size() == 1){
            cout<<"Usage: delep <filepath>"<<endl;
        }else{
            for(int i=2; i<args.size(); i++){
                args[1] += " " + args[i];
            }
            // const char* file_path = args[1].c_str();
            // delep(file_path);
            delep(args[1]);
        }
        return;
    }
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
    // cout<<"command to exec: "<<argv[0]<<endl;
    execvp(argv[0], argv_dup);
    cout<<"Error: Command not found"<<endl;
    exit(1);
}
#endif