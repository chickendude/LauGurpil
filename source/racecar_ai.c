#include <tonc.h>

#include "racecar_ai.h"
#include "camera.h"
#include "checkpoint.h"
#include "race.h"
#include "racecar.h"
#include "track.h"

//#define DEBUG
// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

void update_pos(Racecar *ai_car, Camera *camera);

void set_coordinates(Racecar *ai_car, Race *race);

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void move_ai_car(Racecar *ai_car, Race *race)
{
    // Move car based on speed/angle (and checking terrain)
    if (race->countdown > 0 || ai_car->current_lap > race->laps_total)
    {
        decelerate(ai_car);
        update_pos(ai_car, &race->camera);
        return;
    }

    // Check if car has reached a checkpoint
    const Checkpoint *checkpoint = check_checkpoint(race->track, ai_car);

#ifdef DEBUG
    if (ai_car->overall_standing == 1)
    {
        int x = checkpoint->x - race->camera.x;
        int y = checkpoint->y - race->camera.y;
        if (checkpoint->x - race->camera.x > 240 ||
            checkpoint->x - race->camera.x < -32 ||
            checkpoint->y - race->camera.y > 160 ||
            checkpoint->y - race->camera.y < -32)
            x = 240;
        obj_set_attr(&race->obj_buffer[100],
                     ATTR0_SQUARE | ATTR0_4BPP | y,
                     ATTR1_SIZE_16x16 | x,
                     ATTR2_PALBANK(1) | 4);
    }
#endif

    // Check if car needs to turn to point to next checkpoint
    u16 dx = checkpoint->x - (ai_car->x >> 12);
    u16 dy = checkpoint->y - (ai_car->y >> 12);

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

    // Limit checkpoint speed to car's max speed
    int checkpoint_speed =
            checkpoint->speed >= 200 ? ai_car->max_speed : checkpoint->speed;
    if (ai_car->skill < MAX_AI_SKILL)
        checkpoint_speed -=
                (checkpoint_speed + ((MAX_AI_SKILL - ai_car->skill) << 4)) >> 3;

    if (ai_car->speed >> 7 < checkpoint_speed)
        accelerate(ai_car);
    else
        brake(ai_car);
    update_pos(ai_car, &race->camera);
}

void load_ai_car(Racecar *ai_car, Race *race)
{
    set_coordinates(ai_car, race);
}
// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------

void update_pos(Racecar *ai_car, Camera *camera)
{
    // Update position on screen
    int x = (ai_car->x >> 12) - camera->x;
    int y = (ai_car->y >> 12) - camera->y;
    // Check if car is offscreen and hide it if it is (to avoid wrapping)
    x = (x > 240 || x < -32) ? 240 : x;
    y = (y > 160 || y < -32) ? 160 : y;
    obj_set_pos(ai_car->oam, x - 8, y - 8);
}

void set_coordinates(Racecar *ai_car, Race *race)
{
    const int start_angle = race->track->start_angle;
    const int position = ai_car->overall_standing;
    int x_off, y_off;
    if (start_angle == 0x0000) // up
    {
        x_off = -position % 2;
        y_off = (position / 2) * 2;
    } else if (start_angle == 0x4000) // left
    {
        x_off = (position / 2) * 2;
        y_off = position % 2;
    } else if (start_angle == 0x8000) // down
    {
        x_off = position % 2;
        y_off = -(position / 2) * 2;
    } else // right
    {
        x_off = -(position / 2) * 2;
        y_off = -position % 2;
    }
    int start_x = race->track->start_x + x_off;
    int start_y = race->track->start_y + y_off;
    ai_car->x = start_x << 16;
    ai_car->y = start_y << 16;
}
