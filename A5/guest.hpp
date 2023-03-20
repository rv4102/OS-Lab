#include "data_structures.hpp"
#include <unistd.h>
#define MIN_GUEST_SLEEP_TIME 10
#define MAX_GUEST_SLEEP_TIME 20
#define MIN_STAY_TIME 10
#define MAX_STAY_TIME 30
using namespace std;

void* guest(void* arg);