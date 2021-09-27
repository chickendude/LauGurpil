#ifndef TEST_RACE_H
#define TEST_RACE_H

#include <tonc.h>
#include "types.h"
#include "camera.h"

struct Race
{
    Racecar *car;
    Camera camera;
    const Track *track;
    OBJ_ATTR obj_buffer[128];
};

extern State race_state;


#endif //TEST_RACE_H
