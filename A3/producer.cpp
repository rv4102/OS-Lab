#include "graph.hpp"
#include <ctime>
#include <cstdlib>
using namespace std;

int main(){
    int shmid;
    shmid = shmget(KEY, sizeof(Graph), IPC_CREAT | 0660);
    if(shmid < 0) exit(1);

    Graph* graph = (Graph*)shmat(shmid, NULL, 0);
    srand(time(NULL));
    int m = rand() % 21 + 10;
    



}