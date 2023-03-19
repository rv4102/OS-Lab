#include <iostream>
#include <algorithm>
#include <semaphore.h>
#include <pthread.h>
#include "guest.h"
#include "cleaning_staff.h"

using namespace std;

int n, x, y;
int* priority;


struct Room{
    int current_guest;
    int last_time;
    int current_time;
    int num_guest_since_last_clean;
};

Room* rooms;
sem_t* sems;

int main(){
    cin >> n >> x >> y;
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

    srand(time(NULL));
    random_shuffle(priority, priority+y);
    for(int i=0; i<y; i++){
        cout << priority[i] << " ";
    }

    pthread_t guests[y];
    pthread_t cleaning_staffs[x];
    for(int i=0; i<y; i++){
        pthread_create(&guests[i], NULL, guest, (void*)i);
    }
    for(int i=0; i<x; i++){
        pthread_create(&cleaning_staffs[i], NULL, cleaning_staff, (void*)i);
    }
    for(int i=0; i<y; i++){
        pthread_join(guests[i], NULL);
    }
    for(int i=0; i<x; i++){
        pthread_join(cleaning_staffs[i], NULL);
    }
    return 0; 
}   