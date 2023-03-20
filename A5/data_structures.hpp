#ifndef __DATA_STRUCUTRES__
#define __DATA_STRUCUTRES__ 
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <random>
#include <semaphore.h>
#include <pthread.h>

struct Room{
    int current_guest;
    int last_time;
    int current_time;
    sem_t room_occupancy; // initialized to 2 to denote that 2 people can occupy the room at the same time
    bool cleaned = true;
};

#endif