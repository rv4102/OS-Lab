#include "data_structures.hpp"
#include "guest.hpp"
// #include "cleaning_staff.hpp"
using namespace std;

// global variables
int n, x, y;
int *priority;
Room *rooms;
pthread_t *guests;
pthread_t *cleaning_staffs;
bool is_cleaning = false;

pthread_mutex_t *room_mutexes;

pthread_mutex_t *guest_mutexes;
pthread_cond_t *guest_conds;

pthread_mutex_t *cleaning_mutexes;
pthread_cond_t *cleaning_conds;


int main()
{
    cout << "Enter n, x and y:" << endl;
    cin >> n >> x >> y;

    assert(y > n);
    assert(n > x);
    assert(x > 1);

    priority = new int[y];
    rooms = new Room[n];
    guests = new pthread_t[y];
    cleaning_staffs = new pthread_t[x];

    room_mutexes = new pthread_mutex_t[n];

    guest_mutexes = new pthread_mutex_t[y];
    guest_conds = new pthread_cond_t[y];

    cleaning_mutexes = new pthread_mutex_t[x];
    cleaning_conds = new pthread_cond_t[x];

    for(int i=0; i<y; i++){
        pthread_mutex_init(&guest_mutexes[i], NULL);
        pthread_cond_init(&guest_conds[i], NULL);
    }
    for(int i=0; i<x; i++){
        pthread_mutex_init(&cleaning_mutexes[i], NULL);
        pthread_cond_init(&cleaning_conds[i], NULL);
    }

    // sem_init(&rooms_available, 0, n);
    // sem_init(&room_cleaning, 0, 0); // no room is being cleaned initially
    for (int i = 0; i < y; i++)
    {
        priority[i] = y - i;
    }
    // sem_t sema;
    // cout << sem_init(&sema, 0, 2);

    for (int i = 0; i < n; i++)
    {
        rooms[i].current_guest = -1; // -1->unoccupied
        rooms[i].last_time = 0;
        rooms[i].current_time = 0;
        sem_init(&rooms[i].room_occupancy, 0, 2);
        pthread_mutex_init(&room_mutexes[i], NULL);
    }

    shuffle(priority, priority + y, default_random_engine(time(NULL)));
    for (int i = 0; i < y; i++)
    {
        cout << priority[i] << " ";
    }
    cout << endl;

    for (int i = 0; i < y; i++)
    {
        int *arg = new int;
        *arg = i;
        pthread_create(&guests[i], NULL, guest, (void *)arg);
    }
    // for (int i = 0; i < x; i++)
    // {
    //     int *arg = new int;
    //     *arg = i;
    //     pthread_create(&cleaning_staffs[i], NULL, cleaning_staff, (void *)arg);
    // }
    while(1){
        sleep(1);
    }
    while(1){
        sleep(1);
        int i=-1;
        if(!is_cleaning){
            for(i=0; i<n; i++){
                pthread_mutex_lock(&room_mutexes[i]);
                int current_guest, sem_retval;
                current_guest = rooms[i].current_guest;
                sem_getvalue(&rooms[i].room_occupancy, &sem_retval);
                pthread_mutex_unlock(&room_mutexes[i]);
                if(!(current_guest == -1 && sem_retval == 0)){
                    break;
                }
            }
            if(i == n){
                is_cleaning = true;
                for(int i = 0;i < n;i++){
                    pthread_mutex_lock(&room_mutexes[i]);
                    rooms[i].cleaned = false;
                    pthread_mutex_unlock(&room_mutexes[i]);
                }
                for(int i=0; i<x; i++){
                    pthread_cond_signal(&cleaning_conds[i]);
                }
            }
        }
        else{
            for(i=0; i<n; i++){
                pthread_mutex_lock(&room_mutexes[i]);
                if(!rooms[i].cleaned){
                    pthread_mutex_unlock(&room_mutexes[i]);
                    break;
                }
                pthread_mutex_unlock(&room_mutexes[i]);
            }
            if(i == n){
                is_cleaning = false;
                for(int i = 0;i < n;i++){
                    pthread_mutex_lock(&room_mutexes[i]);
                    sem_post(&rooms[i].room_occupancy);
                    sem_post(&rooms[i].room_occupancy);
                    pthread_mutex_unlock(&room_mutexes[i]);
                }
                for(int i=0; i < y; i++){
                    pthread_cond_signal(&guest_conds[i]);
                }
            }
        }
        

    }
    for (int i = 0; i < y; i++)
    {
        pthread_join(guests[i], NULL);
    }
    // for (int i = 0; i < x; i++)
    // {
    //     pthread_join(cleaning_staffs[i], NULL);
    // }

    delete[] priority;
    delete[] rooms;
    sem_destroy(&rooms_available);
    sem_destroy(&room_cleaning);

    return 0;
}