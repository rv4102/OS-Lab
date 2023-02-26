#include "graph.hpp"
#include <ctime>
#include <cstdlib>
#include <queue>
#include <random>
using namespace std;

int main(){
    int shmid;
    shmid = shmget(KEY, sizeof(Graph), IPC_CREAT | 0660);
    if(shmid < 0) exit(1);

    Graph* graph = (Graph*)shmat(shmid, NULL, 0);
    srand(time(NULL));
    int m = rand() % 21 + 10;
    vector <int> k_values(m);
    for(int i = 0;i<m;i++){
        k_values[i] = rand() % 20 + 1;
    }

    cout<<"Number of nodes before updation: "<<graph->num_of_nodes<<endl;
    cout<<"Number of nodes to be added: "<<m<<endl;
    

    for(int i = 0;i<m;i++){
        cout<<i+1<<" -> ";
        int node_num = graph->num_of_nodes;
        int k = k_values[i];

        cout<<node_num<<" : ";
        random_device rd;
        mt19937 gen(rd());
        discrete_distribution<> dis(graph->degree, graph->degree + graph->num_of_nodes);
        for(int j = 0;j<k;j++){
            int neighbour = dis(gen);
            cout<<neighbour<<" ";
            graph->dnode_push(neighbour, node_num);
        }
        cout<<endl;

    }
    cout<<"Number of nodes after updation: "<<graph->num_of_nodes<<endl;
}