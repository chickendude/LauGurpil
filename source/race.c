#include <tonc.h>

#include "race.h"
#include "camera.h"
#include "constants.h"
#include "race_stats.h"
#include "racecar.h"
#include "racecar_ai.h"
#include "state.h"
#include "text.h"
#include "track.h"
// Sprites
#include "cars.h"
#include "lap_numbers.h"

static Race race;

static u32 car_on_camera;

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

// State definitions
static void initialize(StateType, void *);

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
static void initialize(StateType prev_state, void *parameter)
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
    memcpy32(&pal_obj_mem[7 * 16], lap_numbersPal, 8);

    prepare_text(3, 29);

    RaceData *race_data = (RaceData *) parameter;
    load_cars(&race, race_data->car_data);
    load_track(race_data->track, &race.camera);
    load_timer(&race.timer);
    print_time(se_mem[29], 1, 1, 0, 0, 0);

    race.track = race_data->track;
    race.laps = 0;
    race.laps_total = 3;
    race.laps_remaining = 0;
    race.countdown = 60 * 3;
    for (int i = 0; i < race.laps_total; i++)
    {
        race.lap_times[i] = 0;
    }

    // Car sprite/affine info
    for (int i = 0; i < NUM_AI_CARS + 1; i++)
    {
        obj_set_attr(&race.obj_buffer[i],
                     ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF |
                     ATTR0_AFF_DBL_BIT,
                     ATTR1_SIZE_16x16 | ATTR1_AFF_ID(i),
                     ATTR2_PRIO(1) |
                     ATTR2_PALBANK(race_data->car_data[i]->sprite_id) |
                     ATTR2_ID(race_data->car_data[i]->sprite_id * 4));
        obj_aff_identity((OBJ_AFFINE *) &race.obj_buffer[i * 4]);
    }

    // Load AI cars
    for (int i = 0; i < NUM_AI_CARS; i++)
    {
        load_ai_car(&race.computer_cars[i], &race);
    }

    // Set laps #
    obj_set_attr(&race.obj_buffer[7],
                 ATTR0_SQUARE | ATTR0_4BPP | 10,
                 ATTR1_SIZE_16x16 | 200,
                 ATTR2_PALBANK(7) | 32);

    // * 16 (aka << 4) then shift left 12 because of the 12 point fixed point
    race.car->x = race.track->start_x << 16;
    race.car->y = race.track->start_y << 16;
    race.car->angle = race.track->start_angle;
    race.car->slide_x = lu_sin(race.car->angle);
    race.car->slide_y = lu_cos(race.car->angle);
    obj_set_pos(race.car->oam,
                (race.car->x >> 12) - 8 - race.camera.x,
                (race.car->y >> 12) - 8 - race.camera.y);

    vid_vsync();
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG3 | DCNT_OBJ |
                  DCNT_OBJ_1D;

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

    // Move player car
    handle_input(race.car);
    move_car(&race, race.car);

    // Move AI cars
    for (int i = 0; i < NUM_AI_CARS; i++)
    {
        Racecar *ai_car = &race.computer_cars[i];
        move_ai_car(ai_car, &race);
    }

    update_camera(&race, car_on_camera);
    update_laps();

    // Check if final lap has been completed.
    // We use > because laps start at 1, so if we want 3 laps, race.laps needs
    // to be 4 (start of first lap = 1, second = 2, third = 3, and once third
    // lap completes it will be 4)
    if (race.laps > race.laps_total || key_hit(KEY_START))
    {
        // Remove this state from stack so that when the stats screen returns
        // it goes straight to the previous screen
        pop_state(state_stack, RACE, NULL);
        // Load the race stats screen
        push_state(state_stack, &race_stats_state, RACE, &race);
    }

    // Set player so that they are aligned with the camera
    obj_set_pos(race.car->oam,
                (race.car->x >> 12) - 8 - race.camera.x,
                (race.car->y >> 12) - 8 - race.camera.y);

    if (key_hit(KEY_SELECT))
    {
        pop_state(state_stack, RACE, &race);
    }
    if (key_hit(KEY_R))
    {
        car_on_camera = (car_on_camera + 1) % NUM_CARS;
    }
    if (key_hit(KEY_L))
    {
        car_on_camera = (car_on_camera - 1) % NUM_CARS;
    }
}

void update()
{
    update_tilemap(&race);

    // Show current MPH
    print_speed(se_mem[29], 10, 1, race.car->speed);

    // Update timer every other frame
    if (race.timer.frames & 2)
    {
        print_time(se_mem[29], 1, 1, race.timer.minutes,
                   race.timer.seconds, race.timer.millis);
    }

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
    obj_set_attr(&race.obj_buffer[127],
                 ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(SCREEN_HEIGHT / 2 - 8),
                 ATTR1_SIZE_16x16 | ATTR1_X(SCREEN_WIDTH / 2 - 8),
                 ATTR2_PALBANK(7) | ATTR2_PRIO(0) | sprite_id);

    // Hide sprite if countdown is complete
    if (*countdown == 0)
    {
        obj_set_pos(&race.obj_buffer[127], -16, -16);
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