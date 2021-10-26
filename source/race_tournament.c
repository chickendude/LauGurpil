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
    switch (prev_state)
    {
        // TODO: Load tournament selection first
        case NONE:
            push_state(state_stack, &racecar_select_state, RACE_TOURNAMENT, NULL);
            break;
        case TRACK_SELECT:
            push_state(state_stack, &race_state, RACE_TOURNAMENT, race_data);
            break;
        default:
            pop_state(state_stack, RACE_TOURNAMENT, NULL);
    }
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------