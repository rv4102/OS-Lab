#include "data_structures.hpp"

extern int n;
extern int* priority;
extern Room* rooms;
extern sem_t rooms_available;
extern sem_t room_cleaning;
extern pthread_t* guests;
extern pthread_t* cleaning_staffs;
extern bool is_cleaning;

extern pthread_mutex_t *room_mutexes;
extern pthread_mutex_t *cleaning_mutexes;
extern pthread_cond_t *cleaning_conds;
extern pthread_mutex_t rooms_to_clean_mutex;

extern std::vector<int> rooms_to_clean;


void *cleaning_staff(void *arg);
