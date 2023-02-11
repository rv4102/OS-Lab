#ifndef __PIPELINE_H
#define __PIPELINE_H

#include <string>
#include <vector>

#include "commands.h"
#include "utils.h"
using namespace std;

#define RUNNING 0
#define STOPPED 1
#define DONE 2

class Pipeline {
   public:
    string cmd;             
    vector<string> cmds;  
    bool is_bg;             
    pid_t pgid;             
    int num_active;        
    int status;            
    Pipeline(string& cmd);
    Pipeline(vector<string>& cmds);
    void executePipeline();
};
#endif