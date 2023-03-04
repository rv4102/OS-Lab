#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__
#include "graph.hpp"
#include <time.h>
#include <fstream>

using namespace std;

// node * graph::updateOrder(size_t v, size_t order){
//     // node *newNode = this->adj[v];
//     // newNode->id = v;
//     // newNode->feed_order = order;
//     // newNode->next = NULL;
//     // return newNode;
// }

void graph::init(){
    this->num_nodes=37700;
    for(int i=0; i<NUM_NODES; i++)
        this->adj[i] = NULL;
}

void graph::addEdge(int s, int d){
    // size_t order=0;
    // // random order assigned
    // node *newNode = updateOrder(d, order);
    // newNode->next = this->adj[s];
    // this->adj[s] = newNode;

    // newNode = updateOrder(s, order);
    // newNode->next = this->adj[d];
    // this->adj[d] = newNode;
    if(adj[d]==NULL){
        adj[d] = (node *)malloc(sizeof(node));
        adj[d]->id = d;
        adj[d]->deg = 0;
    }
    if(adj[s]==NULL){
        adj[s] = (node *)malloc(sizeof(node));
        adj[s]->id = s;
        adj[s]->deg = 0;
    }
    srand(time(NULL));
    int order = rand()%2;
    this->adj[d]->feed_order = order;
    this->adj[d]->neeightbors[this->adj[d]->deg] = this->adj[s];
    this->adj[d]->deg++;
    order = rand()%2;
    this->adj[s]->feed_order = order;
    this->adj[s]->neeightbors[this->adj[s]->deg] = this->adj[d];
    this->adj[s]->deg++;
}

void graph::read_graph(char *path){
    ifstream infile(path);
    if(!infile.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }
    int x = 0, y = 0;
    char dummy[10], c; // c consumes the comma
    infile >> dummy;
    while(infile >> x >> c >> y) {
        addEdge(x, y);
    }
    infile.close();
}

#endif