#include <malloc.h>
#include <tonc.h>

#include "race.h"
#include "racecar.h"
#include "track.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

void check_terrain(Racecar *car, const Track *track);

void move_and_check_collisions(Racecar *car, const Track *track);

void slow_down(Racecar *car);

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

void load_car(Race *race)
{
    Racecar *car = race->car;

    // Create car if it doesn't exist
    if (car == NULL)
    {
        car = malloc(sizeof(Racecar));
        race->car = car;
    }

    // Load car defaults
    car->speed = 0;
    car->x = car->y = 0;
    car->oam = &race->obj_buffer[0];
    car->oam->attr2 ^= ATTR2_PRIO(1);

    // Set position and rotation
    obj_set_pos(&race->obj_buffer[0], car->x - 32, car->y - 32);
    obj_aff_rotate((OBJ_AFFINE *) &race->obj_buffer[0], car->angle);
}

void move_car(Race *race)
{
    Racecar *car = race->car;
    car->angle -= key_tri_horz() * 0x0100; // Car turning power

    // Check for acceleration/brakes, favoring brakes over acceleration
    // (if both are pressed at the same time, it will brake)
    if (key_is_down(KEY_B)) car->speed -= 0x0050;
    else if (key_is_down(KEY_A)) car->speed += 0x0050;

    // Slow down the acceleration once the car reaches a certain speed
    if (car->speed > 0x2000) car->speed -= 0x0045;
    // Cap off max speed going forward/backward
    if (car->speed > 0x4000) car->speed = 0x4000; // Max speed
    if (car->speed < -0x1000) car->speed = -0x1000;

    // Check for slowdown/speed up areas
    check_terrain(car, race->track);

    if (race->countdown == 0)
        move_and_check_collisions(car, race->track);

    // Decelerate if we are not accelerating forward
    if (key_is_up(KEY_A) && car->speed > 0)
    {
        car->speed -= 0x0025;
        // Stop car if it goes past 0
        if (car->speed < 0) car->speed = 0;
    } // Decelerate if we are not accelerating backwards
    else if (key_is_up(KEY_B) && car->speed < 0)
    {
        car->speed += 0x0025;
        // Stop car if it goes past 0
        if (car->speed > 0) car->speed = 0;
    }

    obj_aff_rotate((OBJ_AFFINE *) car->oam, car->angle);
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
    if (map_x1 < 0 || map_x2 > track->width || map_y1 < 0 ||
        map_y2 > track->height * track->width)
    {
        tile_1 = 0;
    }

    // Check if any of the tiles = 0 and make sure car speed is fast enough to
    // need slowing down
    if (!(tile_1 && tile_2 && tile_3 && tile_4) && car->speed > 0x0600)
    {
        car->speed -= 0x0090;
    }
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

    car->x += (-car->speed * car->slide_x) >> 12;
    car->y += (-car->speed * car->slide_y) >> 12;
}

void slow_down(Racecar *car)
{
    if (car->speed > 0x0600)
    {
        car->speed -= 0x0090;
    }
}