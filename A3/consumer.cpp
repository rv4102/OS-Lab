#include "graph.hpp"
#include <climits>
using namespace std;
int main(int agrc, char* argv[]){
    if(agrc != 2){
        cout<<"Usage: ./consumer <consumer_no>"<<endl;
        exit(1);
    }
    int shmid = shmget(KEY, sizeof(Graph), IPC_CREAT | 0660);
    if(shmid < 0) exit(1);
    Graph* graph = (Graph*)shmat(shmid, NULL, 0);
    int total_nodes = graph->num_of_nodes;
    int consumer_no = atoi(argv[1]);
    int nodes_per_consumer = total_nodes / 10;
    // assign nodes to consumers
    int start = (consumer_no-1) * nodes_per_consumer;
    int end = consumer_no * nodes_per_consumer - 1;
    if(consumer_no == 10) end = total_nodes - 1;
    // print nodes assigned to consumer
    cout<<"Consumer "<<consumer_no<<" is assigned nodes: ";
    for(int i = start;i<=end;i++){
        cout<<i<<" ";
    }
    cout<<endl;
    // run Djkstra’s shortest path algorithms considering all nodes in its mapped set as the source nodes.
    vector<int> src_nodes;
    for(int i = start;i<=end;i++){
        src_nodes.push_back(i);
    }
    
}