#include <malloc.h>
#include <tonc.h>

#include "racecar.h"
#include "constants.h"
#include "checkpoint.h"
#include "race.h"
#include "track.h"

const RacecarData car1 = {0, 7, 7, 7, "GORRI   "};
const RacecarData car2 = {1, 8, 6, 7, "ZURI    "};
const RacecarData car3 = {2, 5, 8, 8, "ORLEGI  "};
const RacecarData car4 = {3, 8, 8, 5, "HORI    "};
const RacecarData car5 = {4, 2, 10, 9, "MOTO MAX"};
const RacecarData car6 = {5, 5, 7, 9, "URDIN   "};
const RacecarData car7 = {6, 10, 2, 9, "BASATI  "};

const RacecarData *cars[NUM_CARS] = {&car1, &car2, &car3, &car4, &car5, &car6,
                                     &car7};

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

void check_terrain(Racecar *car, const Track *track);

void load_car(Racecar *car, const RacecarData *car_data);

void move_and_check_collisions(Racecar *car, Race *race);

void slow_down(Racecar *car);

/**
 * Calculates the approximate distance between two points, given the (positive)
 * difference between them.
 *
 * @param dx The (absolute) difference between x1 and x2.
 * @param dy The (absolute) difference between y1 and y2.
 * @return The approximate difference between the two points.
 */
int distance_between(int dx, int dy);

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void load_cars(Race *race, const RacecarData **car_data)
{
    // Grab the cars after the one selected to use as the AI cars
    for (int i = 0; i < NUM_CARS_IN_RACE; i++)
    {
        Racecar *car = &race->cars[i];
        car->oam_affine = &((OBJ_AFFINE *) race->obj_buffer)[i];
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
        move_and_check_collisions(car, race);
        car->x += (-car->speed * car->slide_x) >> 12;
        car->y += (-car->speed * car->slide_y) >> 12;
    }

    // Check if car has gone through progress marker
    check_progress_marker(race->track, car);

    obj_aff_rotate(car->oam_affine, car->angle);
}

void update_standing(Race *race, Racecar *car)
{
    /*
     * There are a series of progress markers scattered throughout the map.
     * As a car goes through the track, its "progress_index" is updated.
     * We can calculate a car's position by comparing its "current_lap" and
     * "progress_index" values. If those are the same, then we can estimate
     * how far they are from the next marker.
     */
    int standing = 0;
    for (int i = 0; i < NUM_CARS_IN_RACE; i++)
    {
        Racecar *other_car = &race->cars[i];
        if (other_car == car) continue;

        // Check if the cars are on different laps
        if (other_car->current_lap > car->current_lap) standing++;
        if (other_car->current_lap != car->current_lap) continue;

        // Check if progress marker indices are different
        if (other_car->progress_index > car->progress_index) standing++;
        if (other_car->progress_index != car->progress_index) continue;

        const Checkpoint *marker =
                &race->track->progress_markers[car->progress_index];

        // Calculate distances from next marker
        int other_dx = (other_car->x >> 12) - marker->x;
        int other_dy = (other_car->y >> 12) - marker->y;
        int car_dx = (car->x >> 12) - marker->x;
        int car_dy = (car->y >> 12) - marker->y;

        // Make sure all values are positive
        if (other_dx < 0) other_dx *= -1;
        if (other_dy < 0) other_dy *= -1;
        if (car_dx < 0) car_dx *= -1;
        if (car_dy < 0) car_dy *= -1;

        // Calculate overall distance and check which car is further away
        int other_distance = other_dx + other_dy;
        int car_distance = car_dx + car_dy;
        if (other_distance < car_distance) standing++;
    }
    car->current_standing = standing;
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
    car->name = car_data->name;

    // Load car defaults
    car->speed = 0;
    car->x = car->y = 0;
    car->finish_status = -1; // -1 = behind finish line
    car->oam->attr2 ^= ATTR2_PRIO(1);

    // Clear lap times
    for (int i = 0; i < MAX_LAPS; i++)
    {
        car->lap_times[i] = 0;
    }
    car->finish_time = 0;

    // Set position and rotation
    obj_set_pos(car->oam, car->x - 32, car->y - 32);
    obj_aff_rotate((OBJ_AFFINE *) car->oam, car->angle);
}

int distance_between(int dx, int dy)
{
    if (dx < dy)
    {
        int dx_copy = dx;
        dx = dy;
        dy = dx_copy;
    }

    // If dy == 0, then the hypotenuse and dx are the same length
    if (dy == 0) return dx;

    // Formula found here: https://stackoverflow.com/a/26607206
    int constant = 0b011011011001; // 0.428 in .12 fixed point
    return dx + ((constant * dy * dy / dx) >> 12);
}

void move_and_check_collisions(Racecar *car, Race *race)
{
    // TODO: Don't wrap if off edges of map (currently it will check map tiles
    //  that are off the edge of the map, should set out-of-bounds tiles to 0)
    const Track *track = race->track;
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

    for (int i = 0; i < NUM_CARS_IN_RACE; i++)
    {
        Racecar *other_car = &race->cars[i];
        if (car == other_car) continue;

        int diff_x = (car->x >> 12) - (other_car->x >> 12);
        if (diff_x < 0) diff_x *= -1;

        int diff_y = (car->y >> 12) - (other_car->y >> 12);
        if (diff_y < 0) diff_y *= -1;

        // Pythagorean Theorem:
        // - (x1 - x2)^2 + (y1 - y2)^2 < (r1 + r2)^2
        // - r1/r2 are the radii of each car: 8 pixels
        // - If the hypotenuse (the distance between the two cars) is less than
        // the length of the two radii, then they have collided.
        bool is_collision = (diff_x * diff_x + diff_y * diff_y < 16 * 16);

        if (is_collision) {
            int dist = distance_between(diff_x, diff_y);
            int overlap = (dist - 16) / 2;
            car->x -= overlap * (car->x - other_car->x) / dist;
            car->y -= overlap * (car->y - other_car->y) / dist;
            other_car->x += overlap * (car->x - other_car->x) / dist;
            other_car->y += overlap * (car->y - other_car->y) / dist;
        }
    }

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