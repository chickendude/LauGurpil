#ifndef TEST_RACE_H
#define TEST_RACE_H

#include "racecar.h"

typedef struct {
    Racecar *car;
    OBJ_ATTR obj_buffer[128];
} Race;

void initialize();
void input();

#endif //TEST_RACE_H
