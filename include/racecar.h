#ifndef TEST_RACECAR_H
#define TEST_RACECAR_H

#include "constants.h"
#include "types.h"

struct Racecar {
    // .12 fixed point
    int x, y;

    // .12 fixed point
    // This is the momentum which gradually gets closer to the proper values
    // to push the car in the direction of "angle".
    int slide_x, slide_y;

    // 0x0000-0xFFFF
    u16 angle;

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

    // Object's id in oam, since sprites and affine are
    OBJ_AFFINE *oam_affine;

    // Address in OAM
    OBJ_ATTR *oam;
};

struct RacecarData {
    int sprite_id;
    int max_speed;
    int turning_power, acceleration_power;
};

void load_cars(Race *race, const RacecarData **car_data);
void handle_input(Racecar *car);
void move_car(Race *race, Racecar *car);
// Commands
void accelerate(Racecar *car);
void decelerate(Racecar *car);
void brake(Racecar *car);
void turn(Racecar *car, int direction);

extern const RacecarData *cars[7];
extern const RacecarData car1, car2, car3, car4, car5, car6, car7;
#endif //TEST_RACECAR_H
