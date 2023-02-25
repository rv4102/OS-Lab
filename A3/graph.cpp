#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
typedef struct vertex {
    int id;
    int num_adj_vertices;
    int *adj_vertices;
} Vertex;

int main() {
    int num_vertices = 5;
    int num_edges = 6;
    int adj_vertices[] = {1, 2, 2, 3, 3, 4, 4, 5, 5, 1, 1, 4}; 
    int num_adj_vertices[] = {2, 2, 2, 2, 2};
    int offset = 0;
    int size = sizeof(Vertex) * num_vertices + sizeof(int) * num_edges;
    key_t key = ftok("filename", 'b');
    int shmid = shmget(key, size, IPC_CREAT | 0666);
    char *shared_memory = (char *) shmat(shmid, NULL, 0);
    Vertex *adj_list = (Vertex *) shared_memory;

    // initialize vertices and their adjacent vertices
    for (int i = 0; i < num_vertices; i++) {
        adj_list[i].id = i;
        adj_list[i].num_adj_vertices = num_adj_vertices[i];
        adj_list[i].adj_vertices = (int *) (shared_memory + sizeof(Vertex) * num_vertices + sizeof(int) * offset);

        for (int j = 0; j < num_adj_vertices[i]; j++) {
            adj_list[i].adj_vertices[j] = adj_vertices[offset++];
        }
    }

    // print the adjacency list
    for (int i = 0; i < num_vertices; i++) {
        printf("Vertex %d:", adj_list[i].id);

        for (int j = 0; j < adj_list[i].num_adj_vertices; j++) {
            printf(" %d", adj_list[i].adj_vertices[j]);
        }

        printf("\n");
    }

    shmdt(shared_memory);
    int pid = fork();
    if(pid == 0){
        Vertex* adj_list_child = (Vertex*) shmat(shmid, NULL, 0);
        // print the adjacency list
        for (int i = 0; i < num_vertices; i++) {
            printf("Vertex %d:", adj_list_child[i].id);
            for (int j = 0; j < adj_list_child[i].num_adj_vertices; j++) {
                printf(" %d", adj_list_child[i].adj_vertices[j]);
            }

            printf("\n");
        }
        shmdt(adj_list_child);

    }else{
        wait(NULL);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}
