#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_NODES 5

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Error: could not open file \"" << argv[1] << "\"\n";
        return 1;
    }

    // key_t key = ftok("graph_shared_mem", 'R');
    int shmid = shmget(0x1234, 1024, 0666|IPC_CREAT);
    if (shmid == -1) {
        cerr << "Error: could not create shared memory\n";
        return 1;
    }
    void* shmptr = shmat(shmid, NULL, 0);
    if(shmptr == (void*)-1) {
        cerr << "Error: could not attach to shared memory\n";
        return 1;
    }
    // vector<int>* graph = new (shmptr) vector<int>[MAX_NODES];
    // int x, y;
    // while (infile >> x >> y) {
    //     graph[x].push_back(y);
    //     graph[y].push_back(x);
    // }
    // infile.close();
    // for(int i=0; i<MAX_NODES; i++){
    //     cout<<i<<": ";
    //     if(graph[i].empty()) continue;
    //     cout<<"NOT EMPTY\n";
    //     for(int j=0; j<graph[i].size(); j++){
    //         cout<<graph[i].at(j)<<" ";
    //     }cout<<endl;
    // }
    // if(shmdt(shmptr) == -1) {
    //     cerr << "Error: could not detach from shared memory\n";
    //     return 1;
    // }
    int **graph = (int**)(shmptr);
    int *node_neighbour = (int*) ((char*)(shmptr) + sizeof(int*) * MAX_NODES); 
    for(int i = 0; i < MAX_NODES; i++) {
        graph[i] = &node_neighbour[i * MAX_NODES];
        for(int j = 0; j < MAX_NODES; j++) {
            graph[i][j] = 0;
        }
    }
    int x, y;
    while (infile >> x >> y) {
        graph[x][y] = 1;
        graph[y][x] = 1;
    }

    for(int i=0; i<MAX_NODES; i++){
        cout<<i<<": ";
        for(int j=0; j<MAX_NODES; j++){
            cout<<graph[i][j]<<" ";
        }cout<<endl;
    }

    shmdt(shmptr);

    return 0;
}
