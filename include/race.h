#ifndef TEST_RACE_H
#define TEST_RACE_H

#include <tonc.h>
#include "camera.h"
#include "constants.h"
#include "racecar.h"
#include "timer.h"
#include "types.h"

struct Race
{
    Racecar *car;
    Racecar computer_cars[NUM_AI_CARS];
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
    const RacecarData **car_data;
    const Track *track;
};

extern State race_state;


#endif //TEST_RACE_H
