#include <iostream>
#include <cstring>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

// Node structure for linked list
struct Node {
    int val;
    Node* next;
};

// Graph structure
struct Graph {
    int num_nodes;
    Node** adj_list;
};

// Function to create a new node
Node* create_node(int val) {
    Node* new_node = (Node*)shmat(shmget(IPC_PRIVATE, sizeof(Node), IPC_CREAT | 0666), NULL, 0);
    new_node->val = val;
    new_node->next = NULL;
    return new_node;
}

// Function to add an edge between two nodes
void add_edge(Graph* graph, int x, int y) {
    // Update array with NULL address
    // graph->adj_list[x] = NULL;
    // graph->adj_list[y] = NULL;

    // Traverse linked list to find the end
    Node* node_x = graph->adj_list[x];
    Node* node_y = graph->adj_list[y];
    while (node_x != NULL && node_x->next != NULL) {
        node_x = node_x->next;
    }
    while (node_y != NULL && node_y->next != NULL) {
        node_y = node_y->next;
    }

    // Add node to end of linked list
    if (node_x == NULL) {
        graph->adj_list[x] = create_node(y);
    } else {
        node_x->next = create_node(y);
    }
    if (node_y == NULL) {
        graph->adj_list[y] = create_node(x);
    } else {
        node_y->next = create_node(x);
    }
}

int main() {
    // Create shared memory for graph
    int shmid_graph = shmget(IPC_PRIVATE, sizeof(Graph), IPC_CREAT | 0666);
    Graph* graph = (Graph*)shmat(shmid_graph, NULL, 0);

    // Initialize graph
    graph->num_nodes = 4;
    int shmid_adj_list = shmget(IPC_PRIVATE, sizeof(Node*) * graph->num_nodes, IPC_CREAT | 0666);
    graph->adj_list = (Node**)shmat(shmid_adj_list, NULL, 0);
    memset(graph->adj_list, 0, sizeof(Node*) * graph->num_nodes);
    
    // Add edges to graph
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 2);
    add_edge(graph, 2, 3);

    // Print graph
    for (int i = 0; i < graph->num_nodes; i++) {
        cout << "Node " << i << ": ";
        Node* curr_node = graph->adj_list[i];
        while (curr_node != NULL) {
            cout << curr_node->val << " ";
            curr_node = curr_node->next;
        }
        cout << endl;
    }
    int pid = fork();
    if(pid == 0){
        // attach to shared memory
        graph = (Graph*)shmat(shmid_graph, NULL, 0);
        graph->adj_list = (Node**)shmat(shmid_adj_list, NULL, 0);
        // add_edge(graph, 0, 3);
        // shmdt(graph->adj_list);
        // shmdt(graph);

    }else{
        wait(NULL);
        
        for (int i = 0; i < graph->num_nodes; i++) {
            cout << "Node " << i << ": ";
            Node* curr_node = graph->adj_list[i];
            while (curr_node != NULL) {
                cout << curr_node->val << " ";
                curr_node = curr_node->next;
            }
            cout << endl;
        }

        // Detach shared memory
        shmdt(graph->adj_list);
        shmdt(graph);
        shmctl(shmid_adj_list, IPC_RMID, NULL);
        shmctl(shmid_graph, IPC_RMID, NULL);


    }
    // // add new node to graph
    // graph->num_nodes++;
    // // realloc shared memory
    // Node** temp = graph->adj_list;
    // graph->adj_list = (Node**)shmat(shmget(IPC_PRIVATE, sizeof(Node*) * graph->num_nodes, IPC_CREAT | 0666), NULL, 0);
    // memcpy(graph->adj_list, temp, sizeof(Node*) * (graph->num_nodes - 1));
    // memset(graph->adj_list + (graph->num_nodes - 1), 0, sizeof(Node*));
    // // detach old shared memory
    // shmdt(temp);
    // // Print graph
    // for (int i = 0; i < graph->num_nodes; i++) {
    //     cout << "Node " << i << ": ";
    //     Node* curr_node = graph->adj_list[i];
    //     while (curr_node != NULL) {
    //         cout << curr_node->val << " ";
    //         curr_node = curr_node->next;
    //     }
    //     cout << endl;
    // }
    
    // // Detach shared memory
    // shmdt(graph->adj_list);
    // shmdt(graph);
    // return 0;
}
