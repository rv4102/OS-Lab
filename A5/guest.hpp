#include "data_structures.hpp"
#include <unistd.h>

#define MIN_GUEST_SLEEP_TIME 10
#define MAX_GUEST_SLEEP_TIME 20
#define MIN_STAY_TIME 10
#define MAX_STAY_TIME 30

extern int n;
extern int* priority;
extern Room* rooms;
extern sem_t rooms_available;
extern sem_t room_cleaning;

int gen_random(int a, int b);
void* guest(void* arg);
void check_in(int guest_idx, int stay_time, int room_id);
void check_out(int guest_idx, int room_id);
