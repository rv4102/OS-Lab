#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <fstream>

using namespace std;
#define SHARED_MEM_NAME "/graph_shared_mem"
#define NUM_NODES 5

int graph[NUM_NODES][NUM_NODES];
int main(int argc, char* argv[]) {
    if(argc < 2){
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if(!infile){
        cerr << "Error: Could not open file " << argv[1] << endl;
        return 1;

    }

    // Create shared memory
    int shared_mem_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    if(shared_mem_fd == -1){
        cerr << "Error: Could not create shared memory" << endl;
        return 1;
    }

    ftruncate(shared_mem_fd, NUM_NODES * NUM_NODES * sizeof(int));

    // Map shared memory
    int* shared_mem_ptr = (int*)mmap(NULL, NUM_NODES * NUM_NODES * sizeof(int),
                                      PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_fd, 0);
    if(shared_mem_ptr == MAP_FAILED){
        cerr << "Error: Could not map shared memory" << endl;
        return 1;
    }

    // Read graph from file
    int x, y;
    while(infile >> x >> y){
        graph[x][y] = 1;
        graph[y][x] = 1;
    }

    std::memcpy(shared_mem_ptr, graph, NUM_NODES * NUM_NODES * sizeof(int));

    // Clean up
    munmap(shared_mem_ptr, NUM_NODES * NUM_NODES * sizeof(int));
    close(shared_mem_fd);

    return 0;
}
