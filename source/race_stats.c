#include <tonc.h>
#include "race.h"
#include "race_stats.h"
#include "text.h"
// tiles
#include "lap_numbers.h"

static Race *race;

unsigned char results_txt[] = "RESULTS:\0";

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------
static void initialize(void *parameter);

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

static void initialize(void *parameter)
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
    print_text(se_mem[30], 10, 1, results_txt);
    print_time(se_mem[30], 10, 3, race->timer.minutes, race->timer.seconds,
               race->timer.frames);
}

static void update()
{};

static void input(StateStack *state_stack)
{
    if (key_hit(KEY_A | KEY_START))
    {
        pop_state(state_stack, &race);
    }

}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------