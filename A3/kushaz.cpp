#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
using namespace std;


#define MAX_DNODE 1000           // Max. domain string length
#define DNULL (MAX_DNODE + 1)    // NULL value

typedef struct DNode {
    int value;
    size_t next;
}DNode;

typedef struct Graph {
    DNode pool[MAX_DNODE];      // fixed-size space for nodes
    size_t node_to_head[MAX_DNODE];
    size_t node_to_tail[MAX_DNODE];
    size_t npool;               // used space in pool
    // size_t head;                // global list head
    Graph(){
        for(int i = 0; i < MAX_DNODE; i++){
            node_to_head[i] = DNULL;
            node_to_tail[i] = DNULL;
        }
    }
}Graph;


Graph *graph;

DNode *dnode_alloc(void) {
    if (graph->npool < MAX_DNODE) 
            return &graph->pool[graph->npool++];
    else perror("dnode_alloc : npool > MAX_DNODE");
}

DNode *dnode(size_t index) {
    return (index == DNULL) ? NULL : graph->pool + index;
}

DNode *dnode_next(const DNode *node) {
    return dnode(node->next);
}
void add_dnode(DNode* node,int a,int b){

    node->value = b;
    node->next = DNULL;
    if(graph->node_to_head[a] == DNULL){
        graph->node_to_head[a] = node - graph->pool;
        graph->node_to_tail[a] = node - graph->pool;
    }
    else{
        DNode* temp = dnode(graph->node_to_tail[a]);
        temp->next = node - graph->pool;
        graph->node_to_tail[a] = node - graph->pool;
    }
}
void dnode_push(int a,int b) {
    
    
    DNode *node1 = dnode_alloc();
    DNode *node2 = dnode_alloc();
}

int main(int argc, char* argv[])
{
    int shmid;

    shmid = shmget(IPC_PRIVATE, sizeof(graph), IPC_CREAT | 0660);
    if (shmid < 0) exit(1);

    graph = (Graph*)shmat(shmid, NULL, 0);
    if (graph == (void *) (-1)) exit(1);

    // graph->head = DNULL;
    graph->npool = 0;
    cout << "Before push" << endl;
    dnode_push(1,2);
    dnode_push(1,3);
    dnode_push(2,3);
    dnode_push(2,4);
    cout << "After push" << endl;
    cout << "graph->npool : " << graph->npool << endl;
   for(int i = 0; i < 4; i++){
        cout << i << " : " << endl;
        DNode* temp = dnode(graph->node_to_head[i]);
        while(temp != NULL){
            cout << temp->value << " ";
            temp = dnode_next(temp);
        }
        cout << endl;
    }

    shmdt(graph);

    return 0;
}