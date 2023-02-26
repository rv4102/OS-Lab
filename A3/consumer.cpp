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
    while (1)
    {
        int total_nodes = graph->num_of_nodes;
        int consumer_no = atoi(argv[1]);
        int nodes_per_consumer = total_nodes / 10;
        int start = (consumer_no - 1) * nodes_per_consumer;
        int end = consumer_no * nodes_per_consumer - 1;
        if (consumer_no == 10)
            end = total_nodes - 1;
        // cout << "Consumer " << consumer_no << " is assigned nodes: ";
        // for (int i = start; i <= end; i++)
        // {
        //     cout << i << " ";
        // }
        // cout << endl;
        cout<<"Consumer "<<consumer_no<<" is running"<<endl;
        vector<int> src_nodes;
        for (int i = start; i <= end; i++)
        {
            src_nodes.push_back(i);
        }
        vector<int> dist(total_nodes, INT_MAX);
        vector<int> parent(total_nodes, -1);
        graph->dijkstra(src_nodes, dist, parent);
        ofstream file;
        string file_name = "Consumer_" + to_string(consumer_no) + ".txt";
        file.open(file_name, ios::out);
        for (int i = 0; i < total_nodes; i++)
        {
            file << "Shortest path to node " << i << " is: ";
            vector<int> path;
            int node = i;
            while (node != -1)
            {
                path.push_back(node);
                node = parent[node];
            }
            reverse(path.begin(), path.end());
            for(int i=0; i<path.size()-1; i++){
                file << path[i] << " -> ";
            }
            file << path[path.size()-1];
            file << endl;
        }
        file.close();
        sleep(30);
    }
    shmdt(graph);
    return 0;
}