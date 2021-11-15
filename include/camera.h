#ifndef LAUGURPIL_CAMERA_H
#define LAUGURPIL_CAMERA_H

#include "types.h"

struct Camera {
    signed int x, y;
};

void update_camera(Camera *camera, Racecar *focused_car);

#endif //LAUGURPIL_CAMERA_H
