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
static void initialize(StateType prev_state, void *parameter);

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
    // Save race parameter
    race = (Race *) parameter;

    // Copy number tiles into tile memory
    memcpy32(tile_mem, lap_numbersTiles + 51 * 8,
             (lap_numbersTilesLen) / 4);
    memcpy32(pal_bg_mem, lap_numbersPal, lap_numbersPalLen / 4);

    // enable BG0/Mode 0 (bgs 0-3) and load tiles into character block 0 and
    // put the map into screenblock 30
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_PRIO(1) | BG_REG_32x32 | BG_4BPP;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;

    // clear map out
    for (int i = 0; i < 1024; i++)
    {
        se_mem[30][i] = 0;
    }
    print_text(se_mem[30], 11, 1, "RESULTS:\0");
    print_text(se_mem[30], 10, 5, "LAP TIMES:\0");
    print_time(se_mem[30], 11, 3, race->timer.minutes, race->timer.seconds,
               race->timer.millis);
    for (int i = 0; i < race->laps_total; i++)
    {
        int frames = race->lap_times[i];
        if (i > 0) frames -= race->lap_times[i - 1];
        int minutes = frames / (60 * 60);
        int seconds = (frames - minutes * 3600) / 60;
        int millis = frames % 60;
        print_time(se_mem[30], 11, 7 + i, minutes, seconds, millis);
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