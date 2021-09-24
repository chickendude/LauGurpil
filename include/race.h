#ifndef TEST_RACE_H
#define TEST_RACE_H

#include <tonc.h>
#include "types.h"

struct Race {
    Racecar *car;
    OBJ_ATTR obj_buffer[128];
};

void initialize();
void input();

#endif //TEST_RACE_H
