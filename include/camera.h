#ifndef LAUGURPIL_CAMERA_H
#define LAUGURPIL_CAMERA_H

#include "types.h"

struct Camera {
    signed int x, y;
};

void update_camera(Race *race, int car_on_camera);

#endif //LAUGURPIL_CAMERA_H
