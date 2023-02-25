#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#define MAX_DNODE 180000          // Max. domain string length
#define DNULL (MAX_DNODE + 1)   // NULL value
#define KEY 1235
typedef struct DNode {
    int value;
    size_t next;
}DNode;

typedef struct Graph {
    DNode pool[MAX_DNODE];      // fixed-size space for nodes
    size_t node_to_head[MAX_DNODE];
    size_t node_to_tail[MAX_DNODE];
    int degree[MAX_DNODE];
    size_t npool;               // used space in pool
    int num_of_nodes;
    void init();
    DNode *dnode_alloc(void);
    DNode *dnode(size_t index);
    DNode *dnode_next(const DNode *node);
    void add_dnode(DNode* node,int a,int b);
    void dnode_push(int a,int b);
}Graph;