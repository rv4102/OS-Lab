#include "guest.hpp"

// global variables
extern int n, x, y;
extern int* priority;
extern Room* rooms;
extern sem_t* sems;

int gen_random(int a, int b){
    srand(time(NULL));
    return rand()%(b-a+1)+a;
}

void *guest(void *arg){
    int guest_idx = *(int *)arg;
    while(1){
        int sleep_time = gen_random(MIN_GUEST_SLEEP_TIME, MAX_GUEST_SLEEP_TIME);    
        sleep(sleep_time);
        // request a room
        int stay_time = gen_random(MIN_STAY_TIME, MAX_STAY_TIME);
        int room_id = -1;
        for(int i=0; i<n; i++){
            if(rooms[i].current_guest == -1){
                room_id = i;
                break;
            }
        }
        int current_priority = priority[guest_idx];
        int removed_guest = -1;
        if(room_id == -1){
            for(int i=0; i<n; i++){
                if(current_priority>priority[rooms[i].current_guest]){
                    if(removed_guest == -1){
                        removed_guest = rooms[i].current_guest;
                        room_id = i;
                    }
                    else{
                        if(priority[removed_guest]>priority[rooms[i].current_guest]){
                            removed_guest = rooms[i].current_guest;
                            room_id = i;
                        }
                    }
                }
            }
        } else{
            rooms[room_id].current_guest = guest_idx;
            rooms[room_id].current_time = stay_time;
            rooms[room_id].num_guest_since_last_clean++;
            cout<<"Guest "<<guest_idx<<" enters room "<<room_id<<endl;
            continue;
        }
        if(room_id == -1){
            cout<<"No room available for guest "<<guest_idx<<endl;
        }
        else{
            cout<<"Guest "<<guest_idx<<" enters room "<<room_id<<endl;
            cout<<"Guest "<<removed_guest<<" is removed from room "<<room_id<<endl;
            rooms[room_id].current_guest = guest_idx;
            rooms[room_id].current_time = stay_time;
            rooms[room_id].num_guest_since_last_clean++;
        }
    }
    return NULL;
}
