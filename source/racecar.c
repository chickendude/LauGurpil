#include <malloc.h>
#include <tonc.h>

#include "racecar.h"
#include "constants.h"
#include "race.h"
#include "track.h"

const RacecarData car1 = {0, 7, 7, 7};
const RacecarData car2 = {1, 8, 6, 7};
const RacecarData car3 = {2, 5, 8, 8};
const RacecarData car4 = {3, 8, 8, 5};
const RacecarData car5 = {4, 2, 10, 9};
const RacecarData car6 = {5, 5, 7, 9};
const RacecarData car7 = {6, 10, 2, 9};

const RacecarData *cars[NUM_CARS] = {&car1, &car2, &car3, &car4, &car5, &car6,
                                     &car7};

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

void check_terrain(Racecar *car, const Track *track);

void load_car(Racecar *car, const RacecarData *car_data);

void move_and_check_collisions(Racecar *car, const Track *track);

void slow_down(Racecar *car);

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void load_cars(Race *race, const RacecarData **car_data)
{
    // Grab the cars after the one selected to use as the AI cars
    for (int i = 0; i < NUM_CARS_IN_RACE; i++)
    {
        Racecar *car = &race->cars[i];
        car->oam_affine = &obj_aff_mem[i];
        car->oam = &race->obj_buffer[i];
        car->overall_standing = i;
        car->x = race->track->start_x << 16;
        car->y = race->track->start_y << 16;
        car->angle = race->track->start_angle;
        car->slide_x = lu_sin(car->angle);
        car->slide_y = lu_cos(car->angle);
        load_car(car, car_data[i]);
    }
    race->car = &race->cars[0];
}

void handle_input(Racecar *car)
{
    turn(car, key_tri_horz());

    // Check for acceleration/brakes, favoring brakes over acceleration
    // (if both are pressed at the same time, it will brake)
    if (key_is_down(KEY_B)) brake(car);
    else if (key_is_down(KEY_A)) accelerate(car);

    // Decelerate if we are not accelerating forward
    if (key_is_up(KEY_A) && key_is_up(KEY_B))
    {
        decelerate(car);
    }
}

void brake(Racecar *car)
{
    car->speed -= 0x0050;
}

void decelerate(Racecar *car)
{
    if (car->speed > 0)
    {
        car->speed -= 0x0025;
        // Stop car if it goes past 0
        if (car->speed < 0) car->speed = 0;
    } // Decelerate if we are not accelerating backwards
    else if (car->speed < 0)
    {
        car->speed += 0x0025;
        // Stop car if it goes past 0
        if (car->speed > 0) car->speed = 0;
    }
}

void accelerate(Racecar *car)
{
    car->speed += car->acceleration_power;
}

void turn(Racecar *car, int direction)
{
    car->angle -= direction * car->turning_power;
}

void move_car(Race *race, Racecar *car)
{
    // Slow down the acceleration once the car reaches a certain speed
    if (car->speed > car->max_speed >> 1)
    {
        car->speed -= (car->acceleration_power -
                       (car->acceleration_power >> 3));
    }

    // Cap off max speed going forward/backward
    if (car->speed > car->max_speed) car->speed = car->max_speed; // Max speed
    if (car->speed < -0x1000) car->speed = -0x1000;

    // Check for slowdown/speed up areas
    check_terrain(car, race->track);

    if (race->countdown == 0)
    {
        move_and_check_collisions(car, race->track);
        car->x += (-car->speed * car->slide_x) >> 12;
        car->y += (-car->speed * car->slide_y) >> 12;
    }

    obj_aff_rotate(car->oam_affine, car->angle);
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------

void check_terrain(Racecar *car, const Track *track)
{
    const unsigned char *map = track->tilemap;
    // Convert car coordinates into pixels from the .12 fixed point values
    int car_x = car->x >> 12;
    int car_y = car->y >> 12;

    // Convert the x/y coordinates into map coordinates. If the car isn't tile-
    // aligned (meaning it spans more than one tile) we need to check the tile
    // next to it as well.
    int map_x1 = car_x >> 4;
    int map_x2 = (car_x + 15) >> 4;
    int map_y1 = (car_y >> 4) * track->width;
    int map_y2 = ((car_y + 15) >> 4) * track->width;

    int tile_1 = map[map_y1 + map_x1];
    int tile_2 = map[map_y1 + map_x2];
    int tile_3 = map[map_y2 + map_x1];
    int tile_4 = map[map_y2 + map_x2];

    // Check if we are off the map and set a tile to 0 so that it causes the
    // car to slow down
    if (map_x1 < 0 || map_x2 >= track->width || map_y1 < 0 ||
        map_y2 >= track->height * track->width)
    {
        tile_1 = 0;
    }

    // Check if any of the tiles = 0 and make sure car speed is fast enough to
    // need slowing down
    if (!(tile_1 && tile_2 && tile_3 && tile_4) &&
        car->speed > car->max_speed >> 2)
    {
        car->speed -= 0x0090;
    }
}

void load_car(Racecar *car, const RacecarData *car_data)
{
    // Load chosen car's data
    car->max_speed = 0x3A00 + car_data->max_speed * 0x0100;
    car->turning_power = 0xC0 + car_data->turning_power * 0x05;
    car->acceleration_power = 0x15 + car_data->acceleration_power * 0x05;

    // Load car defaults
    car->speed = 0;
    car->x = car->y = 0;
    car->finish_status = -1; // -1 = behind finish line
    car->oam->attr2 ^= ATTR2_PRIO(1);

    // Set position and rotation
    obj_set_pos(car->oam, car->x - 32, car->y - 32);
    obj_aff_rotate((OBJ_AFFINE *) car->oam, car->angle);
}

void move_and_check_collisions(Racecar *car, const Track *track)
{
    // TODO: Don't wrap if off edges of map (currently it will check map tiles
    //  that are off the edge of the map, should set out-of-bounds tiles to 0)
    const unsigned char *map = track->tilemap;

    // Calculate velocity
    int dx = lu_sin(car->angle);
    int dy = lu_cos(car->angle);
    car->slide_x = (dx + 31 * car->slide_x) >> 5;
    car->slide_y = (dy + 31 * car->slide_y) >> 5;

    int x_off = (car->speed * car->slide_x) >> 12;
    int y_off = (car->speed * car->slide_x) >> 12;
    if ((car->x - x_off) >> 16 >= track->width - 1 ||
        (car->y - y_off) >> 16 >= track->height - 1)
        return;

    // Convert car coordinates into pixels from the .12 fixed point values
    int x = (car->speed * car->slide_x) >> 12;
    int horiz_car_x = (car->x - x) >> 12;
    int horiz_car_y = car->y >> 12;

    // Left
    if (car->x >= 0 && ((car->slide_x > 0 && car->speed > 0) ||
                        (car->slide_x < 0 && car->speed < 0)))
    {
        int map_x = horiz_car_x >> 4;
        int map_y = horiz_car_y >> 4;
        int tile_1 = map[map_y * track->width + map_x];
        int tile_2 = map[((horiz_car_y + 15) >> 4) * track->width + map_x];
        if (tile_1 == 64 || tile_2 == 64)
        {
            car->x = (map_x + 1) << 16;
            car->slide_x = 0;
            slow_down(car);
        }
    }
        // Right
    else if ((car->slide_x < 0 && car->speed > 0) ||
             (car->slide_x > 0 && car->speed < 0))
    {
        int map_x = (horiz_car_x + 15) >> 4;
        int map_y = horiz_car_y >> 4;
        int tile_1 = map[map_y * track->width + map_x];
        int tile_2 = map[((horiz_car_y + 15) >> 4) * track->width + map_x];
        if (tile_1 == 64 || tile_2 == 64)
        {
            car->slide_x = 0;
            slow_down(car);
        }
    }

    int y = (car->speed * car->slide_y) >> 12;
    int vert_car_x = car->x >> 12;
    int vert_car_y = (car->y - y) >> 12;
    // Up
    if ((car->slide_y > 0 && car->speed > 0) ||
        (car->slide_y < 0 && car->speed < 0))
    {
        int map_x = vert_car_x >> 4;
        int map_y = vert_car_y >> 4;
        int tile_1 = map[map_y * track->width + map_x];
        int tile_2 = map[map_y * track->width + ((vert_car_x + 15) >> 4)];
        if (tile_1 == 64 || tile_2 == 64)
        {
            car->slide_y = 0;
            slow_down(car);
        }
    }
        // Down
    else if ((car->slide_y < 0 && car->speed > 0) ||
             (car->slide_y > 0 && car->speed < 0))
    {
        int map_x = vert_car_x >> 4;
        int map_y = (vert_car_y + 16) >> 4;
        int tile_1 = map[map_y * track->width + map_x];
        int tile_2 = map[map_y * track->width +
                         ((vert_car_x + 15) >> 4)];
        if (tile_1 == 64 || tile_2 == 64)
        {
            car->slide_y = 0;
            slow_down(car);
        }
    }
}

void slow_down(Racecar *car)
{
    if (car->speed > 0x0600)
    {
        car->speed -= 0x0090;
    }
}