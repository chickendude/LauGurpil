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

    // 0 = in finish line, -1 = behind, 1 = in front
    int finish_status;

    // #### Stats ####

    /**
     * Max speed the car can reach.
     *
     * Store as .12 fixed point
     */
    int max_speed;

    int turning_power, acceleration_power;

    // Address in OAM
    OBJ_ATTR *oam;
};

struct RacecarData {
    int max_speed;
    int turning_power, acceleration_power;
};

void load_car(Race *race, int car_id);
void move_car(Race *race);

extern const RacecarData *cars[7];
extern const RacecarData car1, car2, car3, car4, car5, car6, car7;
#endif //TEST_RACECAR_H
