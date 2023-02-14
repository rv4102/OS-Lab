#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_NODES 5

// Loads a graph into a shared memory

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    