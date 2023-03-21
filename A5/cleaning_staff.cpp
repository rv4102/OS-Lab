#include "cleaning_staff.hpp"

using namespace std;

void *cleaning_staff(void *arg)
{
    int cleaning_staff_idx = *(int *)arg;
    while (1)
    {
        pthread_mutex_lock(&cleaning_mutexes[cleaning_staff_idx]);
        while (!is_cleaning)
        {
            pthread_cond_wait(&cleaning_conds[cleaning_staff_idx], &cleaning_mutexes[cleaning_staff_idx]);
        }

        pthread_mutex_unlock(&cleaning_mutexes[cleaning_staff_idx]);

        while(1){
            pthread_mutex_lock(&rooms_to_clean_mutex);
            if(rooms_to_clean.size() == 0){
                pthread_mutex_unlock(&rooms_to_clean_mutex);
                break;
            }
            int rand_index = rand() % rooms_to_clean.size();
            int room_idx = rooms_to_clean[rand_index];
            rooms_to_clean.erase(rooms_to_clean.begin() + rand_index);
            pthread_mutex_unlock(&rooms_to_clean_mutex);
            // clean room

            pthread_mutex_lock(&room_mutexes[room_idx]);
            Room room = rooms[room_idx];
            assert(room.current_guest == DIRTY);
            assert(room.cleaned == false);
            pthread_mutex_unlock(&room_mutexes[room_idx]);
            
            // The amount of time to clean a room is proportional to the time it was occupied by previous guests
            int time = room.total_time;
            sleep(time);

            pthread_mutex_lock(&room_mutexes[room_idx]);
            
            // mark the room as cleaned
            rooms[room_idx].cleaned = true;
            rooms[room_idx].total_time = 0;
            
            pthread_mutex_unlock(&room_mutexes[room_idx]);
            cout << "Cleaning Staff " << cleaning_staff_idx << " cleaned room " << room_idx << endl;

        }

    }
    pthread_exit(NULL);
}