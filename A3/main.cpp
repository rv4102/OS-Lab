#include "graph.hpp"
#include <fstream>
#include <time.h>

int main() {
    int shmid;
    shmid = shmget(KEY, sizeof(Graph), IPC_CREAT | 0660);
    cout << "Shmid: " << shmid << endl;
    if (shmid < 0) 
        exit(1);

    Graph* graph = (Graph*)shmat(shmid, NULL, 0);

    if (graph == (void *) (-1)) 
        exit(1);
        
    graph->init();

    ifstream infile("facebook_combined.txt");
    if(!infile.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }
    int x = 0, y = 0;
    while(infile >> x >> y) {
        graph->dnode_push(x,y);
    }
    // for(int i = 1; i <= 4; i++){
    //     cout << i << " : ";
    //     DNode* temp = graph->dnode(graph->node_to_head[i]);
    //     while(temp != NULL){
    //         cout << temp->value << " ";
    //         temp = graph->dnode_next(temp);
    //     }
    //     cout << endl;
    // }

    cout<<"Graph created successfully"<<endl;
    cout<<"Total number of nodes: "<<graph->num_of_nodes<<endl;
    shmdt(graph);

    int producer_pid = fork();
    if(producer_pid == 0){
        // execl("./producer", "./producer", NULL);
        // execvp("./producer.out", NULL);
        execlp("./producer.out", "./producer.out", NULL);
        perror("execlp failed.\n");
        exit(1);
    }
    int consumer_pid[10];
    for(int i=0; i<10; i++){
        consumer_pid[i] = fork();
        if(consumer_pid[i] == 0){
            char command[100];
            // sprintf(command, "./consumer.out %d", i+1);
            execlp("./consumer.out", "./consumer.out", to_string(i+1).c_str(), NULL);
            perror("execlp failed.\n");
            exit(1);
        }
    }
    return 0;
}

