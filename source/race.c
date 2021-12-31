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

#define DEBUG

static Race race;

static unsigned int car_on_camera;

// Used to make sure the stats box at the end of the race is only drawn once
static bool statsbox_displayed;

#define TEXTBOX_SBB 28

#define NUM_LAPS 3

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

void post_race();

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
    clear_textboxes(TEXTBOX_SBB);

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
    race.laps_total = NUM_LAPS;
    race.countdown = 10 * 3;
    race.num_cars_finished = 0;

    load_cars(&race, race_data->car_data);
    load_track(race_data->track, &race.camera);
    print_time(se_mem[29], 1, 1, 0);
    statsbox_displayed = false;

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
                     ATTR2_PRIO(2) |
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
    create_textbox(3, TEXTBOX_SBB, 0, 0, 23, 3);
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
        if (key_hit(KEY_START) || key_hit(KEY_A))
            show_stats_screen(state_stack);
        post_race();
    }

#ifdef DEBUG
    if (race.cars[car_on_camera].current_lap > race.laps_total) post_race();
#endif

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
    // both are currently broken
    if (key_hit(KEY_START)) show_stats_screen(state_stack);
    if (key_hit(KEY_SELECT)) race.car->current_lap++;
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

    // Update timer every other frame until car completes race
    if (race.frames & 2)
    {
        bool all_finished = true;
        for (int i = 0; i < NUM_CARS_IN_RACE; i++)
        {
            all_finished = all_finished && race.cars[i].finish_time > 0;
        }
        if (!all_finished)
            print_time(se_mem[29], 1, 1, race.frames);
    }
    print_number(se_mem[29], 21, 1,
                 race.cars[car_on_camera].current_standing + 1);

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

    int current_lap = race.cars[car_on_camera].current_lap;
    if (current_lap > race.laps_total)
        current_lap = race.laps_total;

    int lap = current_lap == 0 ? 32 :
              current_lap * 4 + 28;
    race.obj_buffer[7].attr2 = ATTR2_PALBANK(7) | lap;
}

/**
 * Transfer to the stats screen and complete the race if it hasn't finished yet.
 */
void show_stats_screen(StateStack *state_stack)
{
    // Show dialog while race is completing
    create_textbox(3, TEXTBOX_SBB, 6, 17, 19, 3);
    print_text(se_mem[29], 7, 18, "FINISHING RACE...");

    // Finish race
    while (race.num_cars_finished < NUM_CARS_IN_RACE)
    {
        race.frames++;
        for (int i = 1; i < NUM_CARS_IN_RACE; i++)
        {
            // Only process cars that haven't finished yet
            if (race.cars[i].finish_time > 0) continue;

            move_ai_car(&race.cars[i], &race);
            move_car(&race, &race.cars[i]);
        }
        update_laps();
    }

    // Remove this state from stack so that when the stats screen returns
    // it goes straight to the previous screen
    pop_state(state_stack, RACE, NULL);
    // Load the race stats screen
    push_state(state_stack, &race_stats_state, RACE, &race);
}

/** Player has completed the race */
void post_race()
{
    decelerate(race.car);
    decelerate(race.car);

    if (race.frames < race.cars[car_on_camera].finish_time + 30) return;

    // Create a dialog box to display the final race results
    if (!statsbox_displayed)
    {
        statsbox_displayed = true;
        create_textbox(3, TEXTBOX_SBB,
                       3, 4,
                       24, 2 + NUM_CARS_IN_RACE);
    }

    // Display all the car times
    for (int i = 0; i < race.num_cars_finished; i++)
    {
        Racecar *car = race.ranking[i];
        print_number(se_mem[29], 4, 5 + i, i + 1);
        print_text(se_mem[29], 6, 5 + i, car->name);
        print_time(se_mem[29], 17, 5 + i, car->finish_time);
    }
}