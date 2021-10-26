#include <tonc.h>
#include "race_tournament.h"
#include "race.h"
#include "racecar_select.h"
#include "state.h"

static RaceData *race_data;

static StateType prev_state;

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

static void initialize(StateType leaving_state, void *parameter);

static void input(StateStack *state_stack);

static void update();

// -----------------------------------------------------------------------------
// Public variable definitions
// -----------------------------------------------------------------------------
State race_tournament_state = {
        &initialize,
        &update,
        &input
};

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void initialize(StateType leaving_state, void *parameter)
{
    // Disable display until we're ready
    REG_DISPCNT = 0;

    prev_state = leaving_state;
}

void update()
{

}

void input(StateStack *state_stack)
{
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------