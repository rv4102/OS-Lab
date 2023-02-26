#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__
#include "graph.hpp"
using namespace std;

DNode * Graph::dnode_alloc(void) {
    if (this->npool < MAX_NODE) 
            return &this->pool[this->npool++];
    else perror("dnode_alloc : npool > MAX_DNODE");
}

DNode * Graph::dnode(size_t index) {
    return (index == DNULL) ? NULL : this->pool + index;
}
void Graph::init(){
    this->num_of_nodes = 0;
    this->npool = 0;
    for(int i = 0; i < MAX_NODE; i++){
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

void Graph::dijkstra(vector<int> sources,vector<int> &dist,vector<int> &parent){
    dist = vector<int>(num_of_nodes,INT_MAX);
    parent = vector<int>(num_of_nodes,-1);
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> pq;
    for(int i = 0; i < sources.size(); i++){
        dist[sources[i]] = 0;
        pq.push(make_pair(sources[i],0));
    }
    while(!pq.empty()){
        pair<int,int> top = pq.top();
        pq.pop();
        int node = top.first;
        int distance = top.second;
        if(distance > dist[node]) continue;
        DNode* temp = dnode(node_to_head[top.first]);
        while(temp != NULL){
            if(distance + 1 < dist[temp->value]){
                dist[temp->value] = distance + 1;
                parent[temp->value] = node;
                pq.push(make_pair(temp->value,distance + 1));
            }
            temp = dnode_next(temp);
        }
    }
}
#endif
