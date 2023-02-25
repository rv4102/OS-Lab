#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX_NODES 100
#define MAX_EDGES 100

using namespace std;

struct Node {
    int value;
    Node* next;
};

struct Graph {
    Node* adj_list[MAX_NODES];
};

struct Edge {
    int src, dest;
};

void create_node(Graph* graph, int node) {
    Node* new_node = new Node;
    new_node->value = node;
    new_node->next = NULL;
    graph->adj_list[node] = new_node;
}

void add_edge(Graph* graph, Edge* edge_list, int* num_edges, int max_num_edges, int x, int y) {
    if (*num_edges >= max_num_edges) {
        cout << "Error: maximum number of edges reached" << endl;
        return;
    }

    // add edge to edge list
    edge_list[*num_edges].src = x;
    edge_list[*num_edges].dest = y;
    (*num_edges)++;

    // add edge to adjacency list of vertex x
    Node* new_node = new Node;
    new_node->value = y;
    new_node->next = NULL;
    Node* cur_node = graph->adj_list[x];
    if (cur_node == NULL) {
        graph->adj_list[x] = new_node;
    } else {
        while (cur_node->next != NULL) {
            cur_node = cur_node->next;
        }
        cur_node->next = new_node;
    }

    // add edge to adjacency list of vertex y
    new_node = new Node;
    new_node->value = x;
    new_node->next = NULL;
    cur_node = graph->adj_list[y];
    if (cur_node == NULL) {
        graph->adj_list[y] = new_node;
    } else {
        while (cur_node->next != NULL) {
            cur_node = cur_node->next;
        }
        cur_node->next = new_node;
    }
}

int main() {
    int shmid;
    key_t key = IPC_PRIVATE;
    size_t size = sizeof(Graph) + sizeof(Edge) * MAX_EDGES;
    int shmflg = IPC_CREAT | 0666;
    Graph* graph;
    Edge* edge_list;
    int* num_edges;

    shmid = shmget(key, size, shmflg);
    if (shmid == -1) {
        cout << "Error: failed to create shared memory segment" << endl;
        return -1;
    }

    graph = (Graph*)shmat(shmid, NULL, 0);
    if (graph == (Graph*)-1) {
        cout << "Error: failed to attach shared memory segment" << endl;
        shmctl(shmid, IPC_RMID, NULL);
        return -1;
    }

    edge_list = (Edge*)(graph + 1);
    num_edges = new int;
    *num_edges = 0;

    // initialize graph with nodes 1, 2, and 3 and edges (1,2), (1,3), and (2,3)
    create_node(graph, 1);
    create_node(graph, 2);
    create_node(graph, 3);
    add_edge(graph, edge_list, num_edges, MAX_EDGES, 1, 2);
    add_edge(graph, edge_list, num_edges, MAX_EDGES, 1, 3);
    add_edge(graph, edge_list, num_edges, MAX_EDGES, 2, 3);

    // print graph
    cout << "Graph:" << endl;
    for (int i = 1; i <= 3; i++) {
        cout << "  " << i << ": ";
        Node* cur_node = graph->adj_list[i];
        while (cur_node != NULL) {
            cout << cur_node->value << " ";
            cur_node = cur_node->next;
        }
        cout << endl;
    }

    // create child process
    pid_t pid = fork();
    if(pid == 0){
        Graph* graph_child = (Graph*)shmat(shmid, NULL, 0);
        if (graph_child == (Graph*)-1) {
            cout << "Error: failed to attach shared memory segment" << endl;
            shmctl(shmid, IPC_RMID, NULL);
            return -1;
        }
        create_node(graph_child, 4);
        add_edge(graph_child, edge_list, num_edges, MAX_EDGES, 1, 4);
        shmdt(graph_child);
    } else {
        wait(NULL);
        // parent process
        cout << "Parent process:" << endl;
        // print graph
        for (int i = 1; i <= 5; i++) {
            cout << "  " << i << ": ";
            Node* cur_node = graph->adj_list[i];
            while (cur_node != NULL) {
                cout << cur_node->value << " ";
                cur_node = cur_node->next;
            }
            cout << endl;
        }
        // print edge list
        cout << "Edge list:" << endl;
        for (int i = 0; i < *num_edges; i++) {
            cout << "  " << edge_list[i].src << " " << edge_list[i].dest << endl;
        }
        shmdt(graph);

        // wait for child process to finish
        shmctl(shmid, IPC_RMID, NULL);
    }

}
