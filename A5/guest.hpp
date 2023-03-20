#include <iostream>
#include <unistd.h>
#include <semaphore.h>

#define MIN_GUEST_SLEEP_TIME 10
#define MAX_GUEST_SLEEP_TIME 20
#define MIN_STAY_TIME 10
#define MAX_STAY_TIME 30
using namespace std;

struct Room{
    int current_guest;
    int last_time;
    int current_time;
    int num_guest_since_last_clean;
};

int gen_random(int a, int b);
void* guest(void* arg);
void check_in(int guest_idx, int stay_time, int room_id);
void check_out(int guest_idx, int room_id);

extern int n;
extern int* priority;
extern Room* rooms;
extern sem_t rooms_available;
extern sem_t room_cleaning;


