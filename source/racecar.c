#include <malloc.h>
#include <tonc.h>

#include "race.h"
#include "racecar.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


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
    car->angle = 0x8000;
    car->slide_x = lu_sin(car->angle);
    car->slide_y = lu_cos(car->angle);
    car->speed = 0;
    car->x = car->y = 0;
    car->oam = &race->obj_buffer[0];

    // Set position and rotation
    obj_set_pos(&race->obj_buffer[0], car->x - 16, car->y - 16);
    obj_aff_rotate((OBJ_AFFINE *) &race->obj_buffer[0], car->angle);
}

void move_car(Racecar *car)
{
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

    // Calculate velocity
    int dx = lu_sin(car->angle);
    int dy = lu_cos(car->angle);
    car->slide_x = (dx + 31 * car->slide_x) >> 5;
    car->slide_y = (dy + 31 * car->slide_y) >> 5;
    car->x += (-car->speed * car->slide_x) >> 12;
    car->y += (-car->speed * car->slide_y) >> 12;

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