#ifndef __PIPELINE_HPP
#define __PIPELINE_HPP

#include <string>
#include <vector>
#include "commands.hpp"
#include "utils.hpp"

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