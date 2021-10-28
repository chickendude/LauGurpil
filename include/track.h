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
    // x1/y1 should be < x2/y2
    const int finish_x1, finish_y1;
    const int finish_x2, finish_y2;
    const unsigned char *tilemap;
};

void load_track(const Track *track, Camera *camera);

void update_tilemap(Race *race);

/**
 * -1 = behind finish line, 0 = in finish line, 1 = beyond finish line
 *
 * @param car The car whose position should be checked
 * @param track The track to check
 * @return int containing finish line status: 0 = in finish line, 1 = beyond
 * finish line, -1 = behind finish line
 */
int is_car_in_finish_line(Racecar *car, const Track *track);

extern const Track* tracks[NUM_TRACKS];

extern const Track track_1, track_2, track_3, track_4;

#endif //TEST_TRACK_H
