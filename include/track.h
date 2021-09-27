#ifndef TEST_TRACK_H
#define TEST_TRACK_H

#include "types.h"

struct Track
{
    int start_x, start_y;
    int start_angle;
    int width, height;
    const unsigned char *tilemap;
};

void load_track(const Track *track, Camera *camera);

void update_tilemap(Race *race);

extern const Track track_1;

#endif //TEST_TRACK_H
