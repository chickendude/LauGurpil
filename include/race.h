#ifndef TEST_RACE_H
#define TEST_RACE_H

#include <tonc.h>
#include "camera.h"
#include "constants.h"
#include "racecar.h"
#include "types.h"

struct Race
{
    Racecar *car;
    Racecar cars[NUM_CARS_IN_RACE];
    Camera camera;
    Camera prev_camera;
    int frames;
    int laps_total;
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
