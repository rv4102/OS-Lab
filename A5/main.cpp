#include <iostream>
#include <algorithm>
#include <semaphore.h>
#include <pthread.h>
#include "guest.h"
#include "cleaning_staff.h"

using namespace std;

int n, x, y;
int* priority;

Room* rooms;
sem_t rooms_available;
sem_t room_cleaning;

int main(){
    cin >> n >> x >> y;
    priority = new int[y];
    rooms = new Room[n];
    sem_init(&rooms_available, 0, n);
    sem_init(&room_cleaning, 0, 0); // no room is being cleaned initially
    for(int i=0; i<y; i++){
        priority[i] = y-i;
    }
    for(int i=0; i<n; i++){
        rooms[i].current_guest = -1; // -1->unoccupied
        rooms[i].last_time = 0;
        rooms[i].current_time = 0;
        rooms[i].num_guest_since_last_clean = 0;
    }

    srand(time(NULL));
    random_shuffle(priority, priority+y);
    for(int i=0; i<y; i++){
        cout << priority[i] << " ";
    }

    pthread_t guests[y];
    pthread_t cleaning_staffs[x];
    for(int i=0; i<y; i++){
        int* arg = new int;
        *arg = i;
        pthread_create(&guests[i], NULL, guest, (void*)arg);
    }
    for(int i=0; i<x; i++){
        int* arg = new int;
        *arg = i;
        pthread_create(&cleaning_staffs[i], NULL, cleaning_staff, (void*)arg);
    }
    for(int i=0; i<y; i++){
        pthread_join(guests[i], NULL);
    }
    for(int i=0; i<x; i++){
        pthread_join(cleaning_staffs[i], NULL);
    }

    delete[] priority;
    delete[] rooms;
    sem_destroy(&rooms_available);
    sem_destroy(&room_cleaning);
    
    return 0; 
}   