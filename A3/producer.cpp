#include "graph.hpp"
#include <ctime>
#include <cstdlib>
#include <queue>
using namespace std;

int main(){
    int shmid;
    shmid = shmget(KEY, sizeof(Graph), IPC_CREAT | 0660);
    if(shmid < 0) exit(1);

    Graph* graph = (Graph*)shmat(shmid, NULL, 0);
    srand(time(NULL));
    int m = rand() % 21 + 10;
    
    priority_queue<pair<int,int>> pq;    
    for(int i = 0;i<graph->num_of_nodes;i++){
        pq.push({graph->degree[i],i});
    }
    cout<<"Number of nodes before updation: "<<graph->num_of_nodes<<endl;
    cout<<"Number of nodes to be added: "<<m<<endl;
    for(int i = 0;i<m;i++){
        int node_num = graph->num_of_nodes;
        int k = rand() % 20 + 1;
        vector <pair<int,int>> neighbours;
        for(int j = 0;j<k;j++){
            neighbours.push_back({pq.top().first,pq.top().second});
            pq.pop();
        }
        for(int j = 0;j<k;j++){
            graph->dnode_push(neighbours[j].second, node_num);
            pq.push({graph->degree[neighbours[j].second],neighbours[j].second});
        }
        pq.push({graph->degree[node_num],node_num});
    }
    cout<<"Number of nodes after updation: "<<graph->num_of_nodes<<endl;
}