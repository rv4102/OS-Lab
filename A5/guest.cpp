#include "guest.hpp"

using namespace std;

/*
1. Guest thread sleeps for 10-20 seconds
2. It checks the semaphores declared for room, if all are locked, it waits for the semaphore to be unlocked
3. If a room is available it sleeps for duration of stay and locks the semaphore for that room
4. If no room is available, the guest thread checks the priority of the guests in all the rooms and removes the guest with lowest priority
5. For whichever guest we find the lowest priority, we send a signal to that semaphore and remove the guest from the room
6. The guest now occupies this room and sleeps for the duration of stay
7. The main thread maintains when each thread went to sleep. It wakes them up after a random time if they are still sleeping, by sending SIGUSR1 
*/


int gen_random(int a, int b){
    srand(time(NULL)*pthread_self());
    return rand()%(b-a+1)+a;
}

void check_in(int guest_idx, int stay_time, int room_id){
    // pthread_mutex_lock(&room_mutexes[room_id]);
    rooms[room_id].current_guest = guest_idx;
    rooms[room_id].current_time = stay_time;
    pthread_mutex_unlock(&room_mutexes[room_id]);
    cout<<"Guest "<<guest_idx<<" enters room "<<room_id<<endl;
    // create time struct
    struct timeval now;
    gettimeofday(&now,NULL);
    struct timespec *stay_time_struct = new struct timespec;
    cout << "stay time: " << stay_time << endl;
    stay_time_struct->tv_sec = now.tv_sec + stay_time;
    pthread_mutex_lock(&guest_mutexes[guest_idx]);
    int ret = pthread_cond_timedwait(&guest_conds[guest_idx], &guest_mutexes[guest_idx], stay_time_struct);
    if(ret == ETIMEDOUT){
        cout << "timed out" << endl;
    }
    else{
        cout << "got signal" << endl;
    }
    pthread_mutex_unlock(&guest_mutexes[guest_idx]);
    // in case we get signal or timer runs out we go here
    // cout<<"Guest "<<guest_idx<<" exits room "<<room_id<<endl;
    check_out(guest_idx, room_id);
}

void check_out(int guest_idx, int room_id){
    pthread_mutex_lock(&room_mutexes[room_id]);
    rooms[room_id].current_guest = -1;
    rooms[room_id].last_time = rooms[room_id].current_time;
    rooms[room_id].current_time = 0;
    pthread_mutex_unlock(&room_mutexes[room_id]);
    cout<<"Guest "<<guest_idx<<" leaves room "<<room_id<<endl;
}

void* guest(void* arg){
    int guest_idx = *(int *)arg;
    while(1){
        pthread_mutex_lock(&guest_mutexes[guest_idx]);
        while(is_cleaning){
            pthread_cond_wait(&guest_conds[guest_idx], &guest_mutexes[guest_idx]);
        }
        pthread_mutex_unlock(&guest_mutexes[guest_idx]);

        // cout << "hello" << endl;

        // sleep initially
        int sleep_time = gen_random(MIN_GUEST_SLEEP_TIME, MAX_GUEST_SLEEP_TIME);    
        sleep(sleep_time);

        // request a room
        int i = -1;
        for(i=0; i<n; i++){
            int room_guest = -1, wait_val = -1;
            // use sem_trywait and try to see if any room is available
            pthread_mutex_lock(&room_mutexes[i]);
            room_guest = rooms[i].current_guest;
            if(room_guest == -1){
                int sem_val;
                sem_getvalue(&rooms[i].room_occupancy, &sem_val);
                cout << "sem val for room " << i << " before trywait is " << sem_val << endl;
                wait_val = sem_trywait(&rooms[i].room_occupancy);
                // int sem_val;
                sem_getvalue(&rooms[i].room_occupancy, &sem_val);
                cout << "sem val for room " << i << " after trywait is " << sem_val << endl;
                // int a;
                // sem_getvalue(&rooms[i].room_occupancy, &a);
                // cout << "Wait val is " << wait_val << " " << a << endl;
            }
            if(room_guest == -1 && wait_val == 0){
                // cout << "inside room available" << endl;
                // room available
                int stay_time = gen_random(MIN_STAY_TIME, MAX_STAY_TIME);
                check_in(guest_idx, stay_time, i);
                // pthread_mutex_unlock(&room_mutexes[i]);
                break;
            }
            pthread_mutex_unlock(&room_mutexes[i]);
        }
        // all rooms have one guest
        if(i == n){
            // cout << "Room unavailable, will try to displace guests" << endl;
            int room_id = -1;
            int guest_priority = priority[guest_idx];
            int removed_guest = -1;
            for(int i=0; i<n; i++){
                int val, room_guest;
                pthread_mutex_lock(&room_mutexes[i]);
                sem_getvalue(&rooms[i].room_occupancy, &val);
                cout << "sem val for displacement "<< i <<" is " << val << endl;
                room_guest = rooms[i].current_guest;
                // cout << "Guest Priority" << guest_priority << " " << priority[room_guest] << "Val :" << val << endl;
                if(guest_priority > priority[room_guest] && val != 0){
                    // found a guest with lower priority
                    room_id = i;
                    removed_guest = room_guest;
                    break;
                }
                pthread_mutex_unlock(&room_mutexes[i]);
            }
            // send signal SIGUSR1 to removed_guest
            if(removed_guest != -1){
                cout << "Guest added after displacing guest " << removed_guest << endl;
                pthread_cond_signal(&guest_conds[removed_guest]);
                int stay_time = gen_random(MIN_STAY_TIME, MAX_STAY_TIME);
                check_in(guest_idx, stay_time, room_id);
            }
        }
        
    }
    return NULL;
}
