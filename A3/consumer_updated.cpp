#include "graph.hpp"
#include <climits>
#include <fstream>
#include <algorithm>
int main(int agrc, char *argv[])
{
    if (agrc != 2)
    {
        cout << "Usage: ./consumer <consumer_no>" << endl;
        exit(1);
    }
    int shmid = shmget(KEY, sizeof(Graph), IPC_CREAT | 0660);
    if (shmid < 0)
        exit(1);
    Graph *graph = (Graph *)shmat(shmid, NULL, 0);
    int prev_num_nodes = graph->num_of_nodes;
    while(1){
        int total_nodes = graph->num_of_nodes;
        int consumer_no = atoi(argv[1]);
        int nodes_per_consumer = total_nodes / 10;
        int start = (consumer_no - 1) * nodes_per_consumer;
        int end = consumer_no * nodes_per_consumer - 1;
        if (consumer_no == 10)
            end = total_nodes - 1;
        vector<int> new_nodes;
        for(int i=prev_num_nodes; i<total_nodes; i++){
            new_nodes.push_back(i);
        }
        vector<int> src_nodes;
        for(auto new_node: new_nodes){
            if(new_node >= start && new_node <= end)
            src_nodes.push_back(new_node);
        }
        if(!src_nodes.empty()){
            vector<int> dist(total_nodes, INT_MAX);
            vector<int> parent(total_nodes, -1);
            priority_queue<vector<int>> pq;
            for(auto src_node: src_nodes){
                pq.push({0, src_node});
                dist[src_node] = 0;
            }
            while(!pq.empty()){
                vector<int> top = pq.top();
                pq.pop();
                int u = top[1];
                int w = top[0];
                if(w > dist[u]) continue;
                DNode* temp = graph->dnode(graph->node_to_head[u]);
                bool updated = false;
                while(temp){
                    if(dist[u] + 1 < dist[temp->value]){
                        updated = true;
                        dist[temp->value] = dist[u] + 1;
                        parent[temp->value] = u;
                        pq.push({dist[temp->value], temp->value});
                    }
                    temp = graph->dnode(temp->next);
                }
                if(!updated){
                    break;
                }
            }
        }
        
    }
    shmdt(graph);
    return 0;
}