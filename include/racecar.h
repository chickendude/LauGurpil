#ifndef TEST_RACECAR_H
#define TEST_RACECAR_H

#include "types.h"

struct Racecar {
    // .12 fixed point
    int x, y;

    // .12 fixed point
    // This is the momentum which gradually gets closer to the proper values
    // to push the car in the direction of "angle".
    int slide_x, slide_y;

    // 0x0000-0xFFFF
    int angle;

    // .12 fixed point
    int speed;

    // #### Stats ####

    /**
     * Max speed the car can reach.
     *
     * Store as .12 fixed point (
     */
    int max_speed;

    int turning_power, acceleration_power;

    // Address in OAM
    OBJ_ATTR *oam;
};

void load_car(Race *race);
void move_car(Racecar *car);

#endif //TEST_RACECAR_H
