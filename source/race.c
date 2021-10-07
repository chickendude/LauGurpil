#include <tonc.h>

#include "race.h"
#include "camera.h"
#include "constants.h"
#include "racecar.h"
#include "state.h"
#include "track.h"
// Sprites
#include "cars.h"
#include "lap_numbers.h"

static Race race;
// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

void initialize();

void input(StateStack *state_stack);

void update();

void show_countdown(int *countdown);

void show_timer(int *countdown);

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
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
    oam_init(race.obj_buffer, 128);

    // Load Car sprites
    memcpy32(tile_mem[4], carsTiles, carsTilesLen / 4);
    memcpy32(pal_obj_mem, carsPal, carsPalLen / 4);

    // Load lap numbers and seconds sprites
    memcpy32(tile_mem[4] + carsTilesLen / 32, lap_numbersTiles,
             lap_numbersTilesLen / 4);
    memcpy32(&pal_obj_mem[4 * 16], lap_numbersPal, lap_numbersPalLen / 4);

    obj_set_attr(race.obj_buffer,
                 ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF | ATTR0_AFF_DBL_BIT,
                 ATTR1_SIZE_16x16 | ATTR1_AFF_ID(0),
                 ATTR2_PALBANK(0) | ATTR2_ID(0));

    obj_aff_identity((OBJ_AFFINE *) &race.obj_buffer[0]);

    load_car(&race);
    load_track(&track_1, &race.camera);
    load_timer(&race.timer, &race.obj_buffer[2], 12, 8);

    race.track = &track_1;
    // * 16 (aka << 4) then shift left 12 because of the 12 point fixed point
    race.car->x = track_1.start_x << 16;
    race.car->y = track_1.start_y << 16;
    race.car->angle = track_1.start_angle;
    race.car->slide_x = lu_sin(race.car->angle);
    race.car->slide_y = lu_cos(race.car->angle);
    race.countdown = 60 * 3;
}

void input(StateStack *state_stack)
{
    if (race.countdown > 0)
    {
        show_countdown(&race.countdown);
    } else
    {
        update_timer(&race.timer);
    }

    move_car(&race);
    update_camera(&race);

    // Set player so that they are aligned with the camera
    obj_set_pos(race.car->oam,
                (race.car->x >> 12) - 8 - race.camera.x,
                (race.car->y >> 12) - 8 - race.camera.y);

    if (key_hit(KEY_SELECT))
    {
        pop_state(state_stack);
    }
}

void update()
{
    update_tilemap(&race);
    REG_BG0HOFS = race.camera.x;
    REG_BG1HOFS = race.camera.x;
    REG_BG0VOFS = race.camera.y;
    REG_BG1VOFS = race.camera.y;
    oam_copy(oam_mem, race.obj_buffer, 128);
}
// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------

void show_countdown(int *countdown)
{
    (*countdown)--;
    int sprite_id = SPRITE(8 + *countdown / 60);
    obj_set_attr(&race.obj_buffer[1],
                 ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(SCREEN_HEIGHT / 2 - 8),
                 ATTR1_SIZE_16x16 | ATTR1_X(SCREEN_WIDTH / 2 - 8),
                 ATTR2_PALBANK(4) | ATTR2_PRIO(0) | sprite_id);

    // Hide sprite if countdown is complete
    if (*countdown == 0)
    {
        obj_set_pos(&race.obj_buffer[1], -16, -16);
    }
}