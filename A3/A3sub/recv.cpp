#include <iostream>
#include <vector>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_NODES 88234

using namespace std;

int main() {
    // create key for shared memory
    key_t key = ftok("graph_shared_mem", 'R');
    if (key == -1) {
        cerr << "Error: could not create key\n";
        return 1;
    }

    // get shared memory ID
    int shmid = shmget(key, 2*MAX_NODES*sizeof(int), 0666);
    if (shmid == -1) {
        cerr << "Error: could not get shared memory ID\n";
        return 1;
    }

    int (*graph)[2] = (int (*)[2])shmat(shmid, 0, 0);
    if( graph == (void*)-1 ) {
        cerr << "Error: could not attach to shared memory\n";
        return 1;
    }

    for(int i = 0; i<MAX_NODES; i++){
        for(int j = 0; j<2; j++){
            cout<<graph[i][j]<<" ";
        }
        cout<<endl;
    }

    shmdt(graph);
    return 0;
}
