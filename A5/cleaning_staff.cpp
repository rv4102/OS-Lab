#include "cleaning_staff.hpp"

// global variables
extern int n, x, y;
extern int* priority;
extern Room* rooms;
extern sem_t* sems;

void* cleaning_staff(void* arg){
    int cleaning_staff_idx = *(int *)arg;
    return NULL;
}