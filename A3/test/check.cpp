#include "graph.hpp"

using namespace std;

int main() {
    int shmid;
    shmid = shmget(KEY, sizeof(Graph), IPC_CREAT | 0660);
    cout << "Shmid: " << shmid << endl;

    if (shmid < 0) 
        exit(1);
    Graph* graph = (Graph*) shmat(shmid, NULL, 0);

    for(int i = 1; i <= 4; i++) {
        cout << i << " : ";
        DNode* temp = graph->dnode(graph->node_to_head[i]);
        while(temp != NULL) {
            cout << temp->value << " ";
            temp = graph->dnode_next(temp);
        }
        cout << endl;
    }
    cout << "Num of nodes: " << graph->num_of_nodes << endl;
    shmdt(graph);
    shmctl(shmid, IPC_RMID, NULL);
}