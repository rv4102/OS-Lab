#include <iostream>
#include <vector>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_NODES 5

using namespace std;

int main() {
    // create key for shared memory
    // key_t key = ftok("graph_shared_mem", 'R');
    // if (key == -1) {
    //     cerr << "Error: could not create key\n";
    //     return 1;
    // }

    // get shared memory ID
    int shmid = shmget(1234, 1024, 0666);
    if (shmid == -1) {
        cerr << "Error: could not get shared memory ID\n";
        return 1;
    }

    void* shmptr = shmat(shmid, NULL, 0);
    if(shmptr == (void*)-1) {
        cerr << "Error: could not attach to shared memory\n";
        return 1;
    }
    vector<int>* graph = static_cast<vector<int>*>(shmptr);
    for(int i=0; i<MAX_NODES; i++){
        cout<<i<<": ";
        if(graph[i].empty()) continue;
        cout<<"NOT EMPTY\n";
        for(int j=0; j<graph[i].size(); j++){
            // cout<<graph[i][j]<<" ";
            cout<<graph[i].at(j)<<" ";
        }cout<<endl;
    }
    if(shmdt(shmptr) == -1) {
        cerr << "Error: could not detach from shared memory\n";
        return 1;
    }
    // attach to shared memory
    // vector<int>* graph = (vector<int>*)shmat(shmid, 0, 0);
    // if (graph == (void*)-1) {
    //     cerr << "Error: could not attach to shared memory\n";
    //     return 1;
    // }


    // if(graph == NULL) {
    //     cout << "NULL" << endl;
    // } else {
    //     // cout << "NOT NULL" << endl;
    //     if(graph[2].empty()) {
    //         cout << "EMPTY" << endl;
    //     } else {
    //         cout<<"NOT EMPTY"<<endl;
    //         // print the graph without seg fault

    //         for(int i=0; i<MAX_NODES; i++){
    //             cout<<i<<": ";
    //             if(graph[i].empty()) continue;
    //             // cout<<graph[i].size()<<endl;
    //             // *graph[i].begin();
    //             cout<<*graph[i].begin()<<endl;
    //             // for(auto node: graph[i]){
    //             //     cout<<node<<" ";
    //             // }cout<<endl;
    //         }
    //     }
        
    // }
    // print graph to console
    // for (int i = 0; i < MAX_NODES; i++) {
    //     if (!graph[i].empty()) {
    //         cout << i << ": ";
    //         for (int j = 0; j < graph[i].size(); j++) {
    //             cout << graph[i][j] << " ";
    //         }
    //         cout << endl;
    //     }
    // }

    // detach from shared memory
    // shmdt(graph);

    // void* shared_mem = shmat(shmid, NULL, 0);
    // cout << (char*)shared_mem << endl;
    // shmdt(shared_mem);
    // shmdt(graph);
    return 0;
}
