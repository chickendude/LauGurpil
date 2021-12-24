#ifndef TEST_RACE_H
#define TEST_RACE_H

#include <tonc.h>
#include "camera.h"
#include "constants.h"
#include "racecar.h"
#include "types.h"

struct Race
{
    // Pointer to the player's car, for easy access. Player should also be the
    // first entry in cars.
    Racecar *car;

    // Array containing all the cars in the race, player + AI cars.
    Racecar cars[NUM_CARS_IN_RACE];
    Camera camera;
    Camera prev_camera;

    // How many frames have passed in the race.
    int frames;

    // Total number of laps in the race.
    int laps_total;

    // Number of frames remaining before the race starts.
    int countdown;

    // Current track being raced on.
    const Track *track;

    // Buffer where all OAM values are written, should be copied to OAM during
    // VBlank.
    OBJ_ATTR obj_buffer[128];
};

struct RaceData
{
    const RacecarData **car_data;
    const Track *track;
};

extern State race_state;


#endif //TEST_RACE_H
