#include <time.h>

#include "common.h"

double get_timestamp() {
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);
    return (double) (timestamp.tv_sec * 1000 + timestamp.tv_nsec / 1000000) / 1000.0;
}
