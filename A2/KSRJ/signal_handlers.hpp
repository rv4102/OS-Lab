#ifndef __SIGNAL_HANDLERS_HPP
#define __SIGNAL_HANDLERS_HPP

#include <sys/types.h>
#include <unistd.h>
#include <map>
#include <vector>
#include "pipeline.hpp"

using namespace std;

extern bool ctrlC, ctrlZ, ctrlD;
extern pid_t fgpid;

extern vector<Pipeline*> stored_pipeline;
extern map<pid_t, int> pid_to_idx;

void reapProcesses(int signum);
void toggleSIGCHLDBlock(int how);
void blockSIGCHLD();
void unblockSIGCHLD();
void waitForForegroundProcess(pid_t pid);
void handle_CTRL(int signum);


#endif