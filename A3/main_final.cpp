#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_NODES 300

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

    key_t key = ftok("graph_shared_mem", 'R');
    int shmid = shmget(0x12345, 2*MAX_NODES*sizeof(int), 0666|IPC_CREAT);

    if (shmid == -1) {
        cerr << "Error: could not create shared memory\n";
        return 1;
    }

    int (*graph)[2];
    graph = (int (*)[2]) shmat(shmid, 0, 0);

    if(graph == (void*)-1) {
        cerr << "Error: could not attach to shared memory\n";
        return 1;
    }


    for(int i = 0; i < MAX_NODES; i++) {
        for(int j = 0; j < 2; j++) {
            graph[i][j] = 0;
        }
    }

    int x, y, cnt=0;
    while (infile >> x >> y) {
        graph[cnt][0] = x;
        graph[cnt][1] = y;
        cnt++;
        if(cnt > MAX_NODES) break;
    }

    for(int i=0; i<cnt; i++){
        cout<<i<<": ";
        for(int j=0; j<2; j++){
            cout<<graph[i][j]<<" ";
        }
        cout<<endl;
    }

    shmdt(graph);

    return 0;
}
