#ifndef __COMMANDS_H
#define __COMMANDS_H
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
#include <map>
#include <sstream>
#include <dirent.h>
using namespace std;
typedef struct command{
    string cmd;
    string inp;
    string out;
    command(vector<string> v){
        cmd = v[0];
        inp = v[1];
        out = v[2];
    }
}COMMAND;
COMMAND handle_input_output_redirection(string cmd);
void redirect_in_out(string inp, string out);
void execute_command(vector<string> args);
pid_t get_parent(pid_t pid);
double get_cpu_usage(pid_t pid);
pid_t detect_malware(vector<pid_t> pids);
vector<pid_t> get_parents(pid_t pid);
int delep(string filename);

#endif
