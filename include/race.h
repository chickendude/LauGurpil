#ifndef TEST_RACE_H
#define TEST_RACE_H

#include <tonc.h>
#include "types.h"
#include "camera.h"
#include "timer.h"

struct Race
{
    Racecar *car;
    Camera camera;
    Camera prev_camera;
    Timer timer;
    int laps;
    int laps_total;
    int laps_remaining;
    int countdown;
    const Track *track;
    OBJ_ATTR obj_buffer[128];
};

extern State race_state;


#endif //TEST_RACE_H
