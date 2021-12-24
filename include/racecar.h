#ifndef TEST_RACECAR_H
#define TEST_RACECAR_H

#include "constants.h"
#include "types.h"

struct Racecar
{
    // Car's position in .12 fixed point (first 12 bits are treated as
    // decimals)
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

    // Determines car's starting position in a race
    int overall_standing;

    // Car's current position in the race
    int current_standing;

    int checkpoint_index;

    // Which progress marker the car has reached
    int progress_index;

    int current_lap;

    int lap_times[MAX_LAPS];

    int finish_time;

    /**
     * In case the user (or some other car) goes backwards across the finish
     * line, we need to track that so they can't go back and forth to cheat.
     */
    int laps_remaining;

    // #### Stats ####

    /**
     * Max speed the car can reach.
     *
     * Store as .12 fixed point
     */
    int max_speed;

    int turning_power, acceleration_power;

    // #### OAM ####

    // Object's id in oam, since sprites and affine are
    OBJ_AFFINE *oam_affine;

    // Address in OAM
    OBJ_ATTR *oam;
};

struct RacecarData
{
    int sprite_id;
    int max_speed;
    int turning_power, acceleration_power;
};

void load_cars(Race *race, const RacecarData **car_data);

void handle_input(Racecar *car);

void move_car(Race *race, Racecar *car);

void update_standing(Race *race, Racecar *car);

// Commands
void accelerate(Racecar *car);

void decelerate(Racecar *car);

void brake(Racecar *car);

void turn(Racecar *car, int direction);

extern const RacecarData *cars[7];
extern const RacecarData car1, car2, car3, car4, car5, car6, car7;
#endif //TEST_RACECAR_H
