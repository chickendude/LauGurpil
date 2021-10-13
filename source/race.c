#include <tonc.h>

#include "race.h"
#include "race_stats.h"
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

// State definitions
static void initialize(void *);

static void input(StateStack *state_stack);

static void update();

// Helper functions
void show_countdown(int *countdown);

void update_laps();

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
static void initialize(void *parameter)
{
    // Turn off display while we load/prepare everything
    REG_DISPCNT = 0;

    oam_init(race.obj_buffer, 128);

    // Load Car sprites
    memcpy32(tile_mem[4], carsTiles, carsTilesLen / 4);
    memcpy32(pal_obj_mem, carsPal, carsPalLen / 4);

    // Load laps numbers, seconds, and letter sprites
    memcpy32(tile_mem[4] + carsTilesLen / 32, lap_numbersTiles,
             lap_numbersTilesLen / 4);
    memcpy32(&pal_obj_mem[4 * 16], lap_numbersPal, lap_numbersPalLen / 4);

    load_car(&race);
    load_track(&track_1, &race.camera);
    load_timer(&race.timer, &race.obj_buffer[2], 12, 8);

    // Car sprite/affine info
    int car_id = *((int *) parameter);
    obj_set_attr(race.obj_buffer,
                 ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF | ATTR0_AFF_DBL_BIT,
                 ATTR1_SIZE_16x16 | ATTR1_AFF_ID(0),
                 ATTR2_PRIO(1) | ATTR2_PALBANK(car_id) | ATTR2_ID(car_id * 4));

    obj_aff_identity((OBJ_AFFINE *) &race.obj_buffer[0]);

    // Set laps #
    obj_set_attr(&race.obj_buffer[7],
                 ATTR0_SQUARE | ATTR0_4BPP | 10,
                 ATTR1_SIZE_16x16 | 200,
                 ATTR2_PALBANK(2) | 32);

    race.track = &track_1;
    race.laps = 0;
    race.laps_total = 3;
    race.laps_remaining = 0;
    race.countdown = 60 * 3;
    // * 16 (aka << 4) then shift left 12 because of the 12 point fixed point
    race.car->x = track_1.start_x << 16;
    race.car->y = track_1.start_y << 16;
    race.car->angle = track_1.start_angle;
    race.car->slide_x = lu_sin(race.car->angle);
    race.car->slide_y = lu_cos(race.car->angle);
    obj_set_pos(race.car->oam,
                (race.car->x >> 12) - 8 - race.camera.x,
                (race.car->y >> 12) - 8 - race.camera.y);

    vid_vsync();
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;

    REG_BG0HOFS = race.camera.x;
    REG_BG1HOFS = race.camera.x;
    REG_BG0VOFS = race.camera.y;
    REG_BG1VOFS = race.camera.y;
    oam_copy(oam_mem, race.obj_buffer, 128);
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
    update_laps();

    // Check if final lap has been completed.
    // We use > because laps start at 1, so if we want 3 laps, race.laps needs
    // to be 4 (start of first lap = 1, second = 2, third = 3, and once third
    // lap completes it will be 4)
    if (race.laps > race.laps_total || key_hit(KEY_START))
    {
        // Remove this state from stack so that when the stats screen returns
        // it goes straight to the previous screen
        pop_state(state_stack, NULL);
        // Load the race stats screen
        push_state(state_stack, &race_stats_state, &race);
    }

    // Set player so that they are aligned with the camera
    obj_set_pos(race.car->oam,
                (race.car->x >> 12) - 8 - race.camera.x,
                (race.car->y >> 12) - 8 - race.camera.y);

    if (key_hit(KEY_SELECT))
    {
        pop_state(state_stack, &race);
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

void update_laps()
{
    // Check car's position ("finish_(line_)status") in relation to finish line
    // (if it is on, before, or after the finish line)
    int finish_status = is_car_in_finish_line(race.car, race.track);
    if (race.car->finish_status == -1 && finish_status == 0)
    {
        // If the user hasn't gone backwards through the course
        if (race.laps_remaining == 0)
        {
            // If it's not the first "lap" (crossing the finish line from the
            // starting point), then we don't want to save the lap time.
            if (race.laps > 0)
            {
                race.lap_times[race.laps - 1] = race.timer.frames;
                race.obj_buffer[7].attr2 += 4;
            }
            race.laps++;
        } else
        {
            race.laps_remaining--;
        }
    } else if (race.car->finish_status == 0 && finish_status == -1)
    {
        race.laps_remaining++;
    }
    race.car->finish_status = finish_status;
}