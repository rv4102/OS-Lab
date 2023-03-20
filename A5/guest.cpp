#include "guest.hpp"

int gen_random(int a, int b){
    srand(time(NULL));
    return rand()%(b-a+1)+a;
}

void check_in(int guest_idx, int stay_time, int room_id){
    rooms[room_id].current_guest = guest_idx;
    rooms[room_id].current_time = stay_time;
    rooms[room_id].num_guest_since_last_clean++;
    cout<<"Guest "<<guest_idx<<" enters room "<<room_id<<endl;
}

void check_out(int guest_idx, int room_id){
    rooms[room_id].current_guest = -1;
    rooms[room_id].last_time = rooms[room_id].current_time;
    rooms[room_id].current_time = 0;
    rooms[room_id].num_guest_since_last_clean = 0;
    cout<<"Guest "<<guest_idx<<" leaves room "<<room_id<<endl;
}

void* guest(void* arg){
    int guest_idx = *(int *)arg;
    while(1){
        int sleep_time = gen_random(MIN_GUEST_SLEEP_TIME, MAX_GUEST_SLEEP_TIME);    
        sleep(sleep_time);
        // request a room
        int stay_time = gen_random(MIN_STAY_TIME, MAX_STAY_TIME);
        int room_id = -1;
        int current_priority = priority[guest_idx];
        int removed_guest = -1;
        sem_wait(&rooms_available);
        for(int i=0; i<n; i++){
            if(rooms[i].current_guest == -1){
                room_id = i;
                break;
            }
        }
        if(room_id == -1){
            // all rooms occupied
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
        }else{
            // free room
            check_in(guest_idx, stay_time, room_id);
            sem_post(&rooms_available);
            sleep(stay_time);
            sem_wait(&rooms_available);
            check_out(guest_idx, room_id);
            sem_post(&rooms_available);
            continue;
        }

        if(room_id == -1){
            cout<<"No room available for guest "<<guest_idx<<endl;
            sem_post(&rooms_available);
        }
        else{
            cout<<"Guest "<<removed_guest<<" is removed from room "<<room_id<<endl;
            check_in(guest_idx, stay_time, room_id);
            sem_post(&rooms_available);
            sleep(stay_time);
            sem_wait(&rooms_available);
            check_out(guest_idx, room_id);
            sem_post(&rooms_available);
        }
    }
    return NULL;
}
