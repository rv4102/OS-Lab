#include "graph.hpp"
using namespace std;
int main(int argc, char* argv[])
{
    int shmid;
    shmid = shmget(KEY, sizeof(Graph), IPC_CREAT | 0660);
    cout << "Shmid: " << shmid << endl;
    if (shmid < 0) exit(1);
    Graph* graph = (Graph*)shmat(shmid, NULL, 0);
    if (graph == (void *) (-1)) exit(1);
    graph->init();
    graph->dnode_push(1,2);
    graph->dnode_push(1,3);
    graph->dnode_push(2,3);
    graph->dnode_push(2,4);
    for(int i = 1; i <= 4; i++){
        cout << i << " : ";
        DNode* temp = graph->dnode(graph->node_to_head[i]);
        while(temp != NULL){
            cout << temp->value << " ";
            temp = graph->dnode_next(temp);
        }
        cout << endl;
    }
    shmdt(graph);
    return 0;
}