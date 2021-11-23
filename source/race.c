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

static unsigned int car_on_camera;

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

void show_stats_screen(StateStack *state_stack);

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
    race.track = race_data->track;
    race.frames = 0;
    load_cars(&race, race_data->car_data);
    load_track(race_data->track, &race.camera);
    print_time(se_mem[29], 1, 1, 0);

    race.laps_total = 1;
    race.countdown = 60 * 3;

    // Car sprite/affine info
    for (int i = 0; i < NUM_CARS_IN_RACE; i++)
    {
        Racecar *car = &race.cars[i];
        car->current_lap = 0;
        car->laps_remaining = 0;
        for (int j = 0; j < race.laps_total; j++)
        {
            car->lap_times[j] = 0;
        }

        obj_set_attr(&race.obj_buffer[i],
                     ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF | ATTR0_AFF_DBL_BIT,
                     ATTR1_SIZE_16x16 | ATTR1_AFF_ID(i),
                     ATTR2_PRIO(1) |
                     ATTR2_PALBANK(race_data->car_data[i]->sprite_id) |
                     ATTR2_ID(race_data->car_data[i]->sprite_id * 4));
        obj_aff_identity((OBJ_AFFINE *) &race.obj_buffer[i * 4]);
    }

    // Load AI cars
    for (int i = 1; i < NUM_CARS_IN_RACE; i++)
    {
        load_ai_car(&race.cars[i], &race);
    }

    // Set laps #
    obj_set_attr(&race.obj_buffer[7],
                 ATTR0_SQUARE | ATTR0_4BPP | 10,
                 ATTR1_SIZE_16x16 | 200,
                 ATTR2_PALBANK(7) | 32);

    // * 16 (aka << 4) then shift left 12 because of the 12 point fixed point
    race.car->x = race.track->start_x << 16;
    race.car->y = race.track->start_y << 16;

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
        race.frames++;
    }

    // Handle player input
    if (race.cars[0].current_lap <= race.laps_total)
    {
        handle_input(race.car);
    } else
    {
        decelerate(race.car);
        decelerate(race.car);
        if (key_hit(KEY_START) || key_hit(KEY_A))
            show_stats_screen(state_stack);
    }

    // Handle AI input
    for (int i = 1; i < NUM_CARS_IN_RACE; i++)
    {
        move_ai_car(&race.cars[i], &race);
    }

    // Move all cars
    for (int i = 0; i < NUM_CARS_IN_RACE; i++)
    {
        move_car(&race, &race.cars[i]);
    }

    // Calculate selected car's current position
    update_standing(&race, &race.cars[car_on_camera]);

    update_camera(&race.camera, &race.cars[car_on_camera]);
    update_laps();

#ifdef DEBUG
    if (key_hit(KEY_START)) show_stats_screen(state_stack);
    if (key_hit(KEY_SELECT)) pop_state(state_stack, RACE, &race);
#endif

    // Set player so that they are aligned with the camera
    obj_set_pos(race.car->oam,
                (race.car->x >> 12) - 8 - race.camera.x,
                (race.car->y >> 12) - 8 - race.camera.y);

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
    print_speed(se_mem[29], 10, 1, race.cars[car_on_camera].speed);

    // Update timer every other frame
    if (race.frames & 2)
    {
        print_time(se_mem[29], 1, 1, race.frames);
        print_number(se_mem[29], 21, 1,
                     race.cars[car_on_camera].current_standing + 1);
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
    for (int i = 0; i < NUM_CARS_IN_RACE; i++)
    {
        check_car_crossed_finish_line(&race, &race.cars[i]);
    }

    int lap = race.cars[car_on_camera].current_lap == 0 ? 32 :
              race.cars[car_on_camera].current_lap * 4 + 28;
    race.obj_buffer[7].attr2 = ATTR2_PALBANK(7) | lap;
}

void show_stats_screen(StateStack *state_stack)
{
    // Remove this state from stack so that when the stats screen returns
    // it goes straight to the previous screen
    pop_state(state_stack, RACE, NULL);
    // Load the race stats screen
    push_state(state_stack, &race_stats_state, RACE, &race);
}