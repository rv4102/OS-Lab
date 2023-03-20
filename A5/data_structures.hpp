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
    int num_guest_since_last_clean;
};

#endif