#include <tonc.h>
#include <malloc.h>

#include "race.h"
#include "racecar.h"
#include "state.h"
#include "track.h"
// Sprites
#include "cars.h"

Race race;
// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

void initialize();

void input(StateStack *state_stack);

void update();

// -----------------------------------------------------------------------------
// Public variable definitions
// -----------------------------------------------------------------------------
State race_state = {
        &initialize,
        &update,
        &input
};

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void initialize()
{
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
    oam_init(race.obj_buffer, 128);

    memcpy32(tile_mem[4], carsTiles, carsTilesLen / 4);
    memcpy32(pal_obj_mem, carsPal, carsPalLen / 4);

    obj_set_attr(race.obj_buffer,
                 ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF | ATTR0_AFF_DBL_BIT,
                 ATTR1_SIZE_16x16 | ATTR1_AFF_ID(0),
                 ATTR2_PALBANK(0) | ATTR2_ID(0));

    obj_aff_identity((OBJ_AFFINE *) &race.obj_buffer[0]);

    load_car(&race);
    load_track();
}

void input(StateStack *state_stack)
{
    move_car(race.car);

    if (key_hit(KEY_B))
    {
        pop_state(state_stack);
    }
}

void update()
{
    oam_copy(oam_mem, race.obj_buffer, 128);
}
// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------