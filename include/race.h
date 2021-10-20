#ifndef TEST_RACE_H
#define TEST_RACE_H

#include <tonc.h>
#include "camera.h"
#include "constants.h"
#include "timer.h"
#include "types.h"

struct Race
{
    Racecar *car;
    Camera camera;
    Camera prev_camera;
    Timer timer;
    int laps;
    int laps_total;
    int laps_remaining;
    int lap_times[MAX_LAPS];
    int countdown;
    const Track *track;
    OBJ_ATTR obj_buffer[128];
};

struct RaceData
{
    RacecarData *car_data;
    const Track *track;
};

extern State race_state;


#endif //TEST_RACE_H
