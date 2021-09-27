#ifndef TEST_TRACK_H
#define TEST_TRACK_H

#include "types.h"

struct Track
{
    int start_x, start_y;
    int width, height;
    const unsigned char *tilemap;
};

void load_track(const Track *track);

extern const Track track_1;

#endif //TEST_TRACK_H
