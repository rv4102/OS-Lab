#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>

using namespace std;

#define MAX_LIST 180000
#define MAX_NODE 10000
#define DNULL (MAX_LIST + 1)   // NULL value
#define KEY 1235

typedef struct DNode {
    int value;
    size_t next;
} DNode;

typedef struct Graph {
    // data members
    DNode pool[MAX_LIST];      // fixed-size space for nodes
    size_t node_to_head[MAX_NODE];
    size_t node_to_tail[MAX_NODE];
    int degree[MAX_NODE];       // stores degree of each node
    size_t npool;               // used space in pool
    int num_of_nodes;

    // functions
    void init();
    DNode *dnode_alloc(void);
    DNode *dnode(size_t index);
    DNode *dnode_next(const DNode *node);
    void add_dnode(DNode* node, int a, int b);
    void dnode_push(int a, int b);
    void dijkstra(vector<int> sources, vector<int> &dist, vector<int> &parent);
    void optimized_dijkstra(vector<int> sources, vector<int> &dist, vector<int> &parent);
    void propagate(vector<int> &dist, vector<int> &parent, int node, int distance);
    void update_new_nodes(vector<int> &dist, vector<int> &parent,vector<int> new_nodes);
    void propagate_new_nodes(vector<int> &dist, vector<int> &parent,vector<int> new_nodes);


} Graph;