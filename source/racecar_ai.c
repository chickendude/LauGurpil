#include <tonc.h>

#include "racecar_ai.h"
#include "camera.h"
#include "race.h"
#include "racecar.h"
#include "track.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void move_ai_car(Racecar *ai_car, Race *race)
{
    u16 dx = (race->car->x >> 12) - (ai_car->x >> 12);
    u16 dy = (race->car->y >> 12) - (ai_car->y >> 12);

    u16 angle = ArcTan2(-dy, -dx);
    u16 car_angle = ai_car->angle;
    if (angle > 0x8000)
    {
        car_angle -= 0x8000;
        angle -= 0x8000;
    }

    if (car_angle > angle && car_angle < angle + 0x8000)
    {
        turn(ai_car, 1);
    } else if (car_angle != angle)
    {
        turn(ai_car, -1);
    }

    accelerate(ai_car);

    // Move car based on speed/angle (and checking terrain)
    move_car(race, ai_car);

    // Update position on screen
    int x = (ai_car->x >> 12) - race->camera.x;
    int y = (ai_car->y >> 12) - race->camera.y;
    // Check if car is offscreen and hide it if it is (to avoid wrapping)
    x = (x > 240 || x < -32) ? 240 : x;
    y = (y > 160 || y < -32) ? 160 : y;
    obj_set_pos(ai_car->oam, x - 8, y - 8);
}

void load_ai_car(Racecar *ai_car, Race *race)
{
    int start_x = race->track->start_x + ((ai_car->overall_standing) / 2);
    int start_y = race->track->start_y + ((ai_car->overall_standing) % 2);
    ai_car->x = start_x << 16;
    ai_car->y = start_y << 16;
    ai_car->angle = race->track->start_angle;
    ai_car->slide_x = lu_sin(race->car->angle);
    ai_car->slide_y = lu_cos(race->car->angle);
    obj_set_pos(ai_car->oam,
                (race->car->x >> 12) - 8 - race->camera.x,
                (race->car->y >> 12) - 8 - race->camera.y);
}
// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------