#include "graph.hpp"
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    int shmid;
    shmid = shmget(KEY, sizeof(Graph), IPC_CREAT | 0660);
    cout << "Shmid: " << shmid << endl;

    if (shmid < 0) 
        exit(1);

    Graph* graph = (Graph*)shmat(shmid, NULL, 0);
    if (graph == (void *) (-1)) 
        exit(1);

    graph->init();

    ifstream infile("facebook_combined.txt");
    if(!infile.is_open()){
        cout << "Error opening file" << endl;
        exit(1);
    }
    int x = 0, y = 0;
    while(infile >> x >> y){
        graph->dnode_push(x,y);
    }
    // for(int i = 1; i <= 4; i++){
    //     cout << i << " : ";
    //     DNode* temp = graph->dnode(graph->node_to_head[i]);
    //     while(temp != NULL){
    //         cout << temp->value << " ";
    //         temp = graph->dnode_next(temp);
    //     }
    //     cout << endl;
    // }

    cout<<"Graph created successfully"<<endl;
    cout<<"Total number of nodes: "<<graph->num_of_nodes<<endl;
    shmdt(graph);
    return 0;
}