#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

#define NUM_NODES 38000
#define WALL_MAX 25
#define FEED_MAX 25
#define READ_THREADS 10
#define PUSH_THREADS 25
#define MAX_DEG 9458
typedef struct {
    size_t user_id;
    size_t action_id;
    size_t action_type;
    time_t timestamp;
} action;

typedef struct node {
    size_t id;
    // // if we use deque we need to create constructors
    // deque<action> wall;
    // deque<action> feed;
    action wall[WALL_MAX], feed[FEED_MAX];
    size_t wall_curr=0, feed_curr=0;
    
    // 0 for priority based, 1 for chronological
    size_t feed_order; 
    // node *next;
    int deg=0;
    node* neeightbors[MAX_DEG+1];
} node;

typedef struct graph {
    node *adj[NUM_NODES];
    size_t num_nodes=0;
    void init();
    // node *updateOrder(size_t v, size_t order);
    void addEdge(int s, int d);
    void read_graph(char *path);

} graph;
