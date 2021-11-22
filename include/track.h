#ifndef TEST_TRACK_H
#define TEST_TRACK_H

#include "constants.h"
#include "types.h"

struct Track
{
    const char title[15];
    int start_x, start_y;
    int start_angle;
    int width, height;
    int num_checkpoints;
    const Checkpoint *checkpoints;
    int num_progress_markers;
    const Checkpoint *progress_markers;
    // x1/y1 should be < x2/y2
    const int finish_x1, finish_y1;
    const int finish_x2, finish_y2;
    const unsigned char *tilemap;
};

void load_track(const Track *track, Camera *camera);

void update_tilemap(Race *race);

void check_car_crossed_finish_line(Race *race, Racecar *car);

extern const Track* tracks[NUM_TRACKS];

extern const Track track_1, track_2, track_3, track_4;

#endif //TEST_TRACK_H
