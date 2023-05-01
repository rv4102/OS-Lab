#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#define SHARED_MEM_NAME "/graph_shared_mem"
#define NUM_NODES 5

int main() {
    // Open shared memory
    int shared_mem_fd = shm_open(SHARED_MEM_NAME, O_RDONLY, 0666);

    // Map shared memory
    int* shared_mem_ptr = (int*)mmap(NULL, NUM_NODES * NUM_NODES * sizeof(int),
                                      PROT_READ, MAP_SHARED, shared_mem_fd, 0);

    // Read graph from shared memory
    int graph[NUM_NODES][NUM_NODES];
    std::memcpy(graph, shared_mem_ptr, NUM_NODES * NUM_NODES * sizeof(int));

    // Print graph
    std::cout << "Graph:" << std::endl;
    for (int i = 0; i < NUM_NODES; i++) {
        for (int j = 0; j < NUM_NODES; j++) {
            std::cout << graph[i][j] << " ";
        }
        std::cout << std::endl;
    }

    // Clean up
    munmap(shared_mem_ptr, NUM_NODES * NUM_NODES * sizeof(int));
    close(shared_mem_fd);

    return 0;
}
