#include "graph.hpp"
using namespace std;

DNode * Graph::dnode_alloc(void) {
    if (this->npool < MAX_DNODE) 
            return &this->pool[this->npool++];
    else perror("dnode_alloc : npool > MAX_DNODE");
}

DNode * Graph::dnode(size_t index) {
    return (index == DNULL) ? NULL : this->pool + index;
}
void Graph::init(){
    this->num_of_nodes = 0;
    this->npool = 0;
    for(int i = 0; i < MAX_DNODE; i++){
        this->degree[i] = 0;
        this->node_to_head[i] = DNULL;
        this->node_to_tail[i] = DNULL;
    }
}
DNode * Graph::dnode_next(const DNode *node) {
    return dnode(node->next);
}
void Graph::add_dnode(DNode* node,int a,int b){
    node->value = b;
    node->next = DNULL;
    if(this->node_to_head[a] == DNULL){
        this->num_of_nodes++;
        this->degree[a] = 1;
        this->node_to_head[a] = node - this->pool;
        this->node_to_tail[a] = node - this->pool;
    }
    else{
        this->degree[a]++;
        DNode* temp = dnode(this->node_to_tail[a]);
        temp->next = node - this->pool;
        this->node_to_tail[a] = node - this->pool;
    }
}
void Graph::dnode_push(int a,int b) {
    DNode *node1 =dnode_alloc();
    DNode *node2 = dnode_alloc();
    add_dnode(node1,a,b);
    add_dnode(node2,b,a);
}


