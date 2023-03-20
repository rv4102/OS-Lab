#include "data_structures.hpp"
#include "guest.hpp"
#include "cleaning_staff.hpp"

using namespace std;

// global variables
int n, x, y;
int* priority;
Room* rooms;
sem_t* sems;

int main(){
    cout << "Enter n, x and y:" << endl;
    cin >> n >> x >> y;

    assert(y>n);
    assert(n>x);
    assert(x>1);

    priority = new int[y];
    rooms = new Room[n];
    sems = new sem_t[n];
    for(int i=0; i<y; i++){
        priority[i] = y-i;
    }
    for(int i=0; i<n; i++){
        rooms[i].current_guest = -1; // -1->unoccupied
        rooms[i].last_time = 0;
        rooms[i].current_time = 0;
        rooms[i].num_guest_since_last_clean = 0;
        sem_init(&sems[i], 0, 1);       // binary semaphore
    }

    shuffle(priority, priority+y, default_random_engine(time(NULL)));
    for(int i=0; i<y; i++){
        cout << priority[i] << " ";
    }
    cout << endl;

    pthread_t guests[y];
    pthread_t cleaning_staffs[x];
    for(int i=0; i<y; i++){
        pthread_create(&guests[i], NULL, guest, (void *)&i);
    }
    for(int i=0; i<x; i++){
        pthread_create(&cleaning_staffs[i], NULL, cleaning_staff, (void *)&i);
    }
    for(int i=0; i<y; i++){
        pthread_join(guests[i], NULL);
    }
    for(int i=0; i<x; i++){
        pthread_join(cleaning_staffs[i], NULL);
    }
    return 0; 
}   