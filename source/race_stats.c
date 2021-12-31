#include <tonc.h>
#include "race.h"
#include "race_stats.h"
#include "text.h"
// tiles
#include "lap_numbers.h"

static Race *race;

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------
static void initialize(StateType leaving_state, void *parameter);

static void input(StateStack *state_stack);

static void update();

// -----------------------------------------------------------------------------
// Public variable definitions
// -----------------------------------------------------------------------------
State race_stats_state = {
        &initialize,
        &update,
        &input
};

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

static void initialize(StateType prev_state, void *parameter)
{
    // Save race racecar_id
    race = (Race *) parameter;

    prepare_text(3, 30);

    // enable BG0/Mode 3 (bgs 0-3)
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG3;

    // clear map out
    for (int i = 0; i < 1024; i++)
    {
        se_mem[30][i] = 0;
    }
    print_text(se_mem[30], 11, 1, "RESULTS:\0");
    print_text(se_mem[30], 10, 5, "LAP TIMES:\0");
    for (int i = 0; i < race->laps_total; i++)
    {
        int frames = race->car->lap_times[i];
        if (i > 0) frames -= race->car->lap_times[i - 1];
        print_time(se_mem[30], 11, 7 + i, frames);
    }
    print_time(se_mem[30], 11, 3, race->car->lap_times[NUM_LAPS - 1]);

    print_text(se_mem[30], 11, 11, "AI TIMES:");
    for (int i = 0; i < NUM_AI_CARS; i++)
    {
        Racecar *car = &race->cars[i + 1];
        print_time(se_mem[30], 11, 13 + i, car->lap_times[race->laps_total - 1]);
    }
}

static void update()
{};

static void input(StateStack *state_stack)
{
    if (key_hit(KEY_A | KEY_START))
    {
        pop_state(state_stack, RACE_STATS, &race);
    }

}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------