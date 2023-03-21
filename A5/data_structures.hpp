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
#include <unistd.h>
#include <vector>
#include <random>

#define EMPTY -1
#define DIRTY -2

struct Room{
    int current_guest;
    int total_time;
    sem_t room_occupancy; // initialized to 2 to denote that 2 people can occupy the room at the same time
    bool cleaned = true;
};

#endif