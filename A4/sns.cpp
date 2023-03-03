#include "graph.hpp"

int main(){
    graph *g = (graph *)malloc(sizeof(graph));
    g->init();
    char filename[25] = "musae_git_edges.csv";
    g->read_graph(filename);

    return 0;
}