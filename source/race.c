#include <tonc.h>
#include <malloc.h>

#include "race.h"
#include "racecar.h"
// Sprites
#include "cars.h"

Race race;
// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void initialize()
{
    // TODO: Move vvv to race.c "load_car"
    Racecar *car = malloc(sizeof(Racecar));
    car->angle = 0x8000;
    car->slide_x = lu_sin(car->angle);
    car->slide_y = lu_cos(car->angle);
    car->speed = 0;
    car->x = car->y = 0;

    race.car = car;

    REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;
    oam_init(race.obj_buffer, 128);

    memcpy32(tile_mem[4], carsTiles, carsTilesLen / 4);
    memcpy32(pal_obj_mem, carsPal, carsPalLen / 4);

    obj_set_attr(race.obj_buffer,
                 ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF | ATTR0_AFF_DBL_BIT,
                 ATTR1_SIZE_16x16 | ATTR1_AFF_ID(0),
                 ATTR2_PALBANK(0) | ATTR2_ID(0));

    obj_aff_identity((OBJ_AFFINE *) &race.obj_buffer[0]);
    obj_set_pos(&race.obj_buffer[0], car->x, car->y);
    obj_aff_rotate((OBJ_AFFINE *) &race.obj_buffer[0], car->angle);
    oam_copy(oam_mem, race.obj_buffer, 128);
}

void input()
{
    Racecar *car = race.car;
    vid_vsync();
    oam_copy(oam_mem, race.obj_buffer, 128);

    key_poll();

    // TODO: Move vvv to race.c
    car->angle -= key_tri_horz() * 0x0100; // Car turning power
    car->speed += key_tri_vert() * 0x0050; // Acceleration power
    if (car->speed > 0x2000) car->speed = 0x2000; // Max speed
    if (car->speed < -0x2000) car->speed = -0x2000;

    // Calculate velocity
    int dx = lu_sin(car->angle);
    int dy = lu_cos(car->angle);
    car->slide_x = (dx + 31 * car->slide_x) >> 5;
    car->slide_y = (dy + 31 * car->slide_y) >> 5;
    car->x += (car->speed * car->slide_x) >> 12;
    car->y += (car->speed * car->slide_y) >> 12;

    // Decelerate if we are not accelerating forward
    if (key_is_up(KEY_UP) && car->speed < 0)
    {
        car->speed += 0x0025;
        // Stop car if it goes past 0
        if (car->speed > 0) car->speed = 0;
    } // Decelerate if we are not accelerating backwards
    else if (key_is_up(KEY_DOWN) && car->speed > 0)
    {
        car->speed -= 0x0025;
        // Stop car if it goes past 0
        if (car->speed < 0) car->speed = 0;
    }

    obj_set_pos(&race.obj_buffer[0], (car->x >> 12) - 8, (car->y >> 12) - 8);
    obj_aff_identity((OBJ_AFFINE *) &race.obj_buffer[0]);
    obj_aff_rotate((OBJ_AFFINE *) &race.obj_buffer[0], car->angle);
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------