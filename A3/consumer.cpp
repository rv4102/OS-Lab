#include "graph.hpp"
#include <climits>
#include <fstream>
#include <algorithm>

int main(int argc, char *argv[]) {
    // if (agrc != 2) {
    //     cout << "Usage: ./consumer <consumer_no>" << endl;
    //     exit(1);
    // }
    if(argc < 2){
        cout<<"Usage: ./consumer <consumer_no> -optimize"<<endl;
        exit(1);
    }
    bool optimize = false;
    int consumer_no = atoi(argv[1]);
    if(argc == 3){
        optimize = true;
    }

    int shmid = shmget(KEY, sizeof(Graph), IPC_CREAT | 0660);
    if (shmid < 0)
        exit(1);

    Graph *graph = (Graph *)shmat(shmid, NULL, 0);
    vector<int> dist(MAX_NODE, INT_MAX);
    vector<int> parent(MAX_NODE, -1);
    int prev_nodes = 0;

    while (1) {
        if(!optimize) prev_nodes = 0;
        sleep(30);
        vector<int> src_nodes;
        int total_nodes = graph->num_of_nodes;
        
        int nodes_per_consumer = (total_nodes - prev_nodes) / 10;
        int start = prev_nodes + (consumer_no - 1) * nodes_per_consumer;
        int end = prev_nodes + consumer_no * nodes_per_consumer - 1;
        if (consumer_no == 10)
            end = total_nodes - 1;
        cout<<"Consumer "<<consumer_no<<" is running"<<" with start: "<<start<<" "<<"end: "<<end<<endl;
        for (int i = start; i <= end; i++) {
            // cout<<i<<" ";
            src_nodes.push_back(i);
        }

        graph->optimized_dijkstra(src_nodes, dist, parent);
        // cout<<"prev_nodes: "<<prev_nodes<<" "<<"total_nodes: "<<total_nodes<<endl;
        if(optimize) {
            vector<int> new_nodes;
            for(int i = prev_nodes; i < total_nodes; i++) {
                if(i < start || i > end)
                    new_nodes.push_back(i);
            }
            // graph->update_new_nodes(new_nodes, dist, parent);
            graph->propagate_new_nodes(dist, parent, new_nodes);
        }
        // cout<<"Consumer "<<consumer_no<<" is done"<<endl;
        prev_nodes = total_nodes;
        ofstream file;
        string file_name = "Consumer_" + to_string(consumer_no) + ".txt";
        file.open(file_name, ios::out);
        for (int i = 0; i < total_nodes; i++) {
            file << "Shortest path to node " << i << " is: ";
            vector<int> path;
            int node = i;
            while (node != -1) {
                path.push_back(node);
                node = parent[node];
            }
            reverse(path.begin(), path.end());
            for(int i=0; i<path.size()-1; i++) {
                file << path[i] << " -> ";
            }
            file << path[path.size()-1];
            file << endl;
        }
        file.close();
        // sleep(11);
    }
    shmdt(graph);
    return 0;
}