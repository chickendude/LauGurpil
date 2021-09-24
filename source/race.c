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
    load_car(&race);

    REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;
    oam_init(race.obj_buffer, 128);

    memcpy32(tile_mem[4], carsTiles, carsTilesLen / 4);
    memcpy32(pal_obj_mem, carsPal, carsPalLen / 4);

    obj_set_attr(race.obj_buffer,
                 ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF | ATTR0_AFF_DBL_BIT,
                 ATTR1_SIZE_16x16 | ATTR1_AFF_ID(0),
                 ATTR2_PALBANK(0) | ATTR2_ID(0));

    obj_aff_identity((OBJ_AFFINE *) &race.obj_buffer[0]);
}

void input()
{
    vid_vsync();
    oam_copy(oam_mem, race.obj_buffer, 128);

    key_poll();

    move_car(race.car);
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------