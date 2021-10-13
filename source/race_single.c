#include <tonc.h>
#include "race_single.h"
#include "race.h"
#include "racecar_select.h"
#include "state.h"

int selected_car;

StateType leaving_state;

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

static void initialize(StateType ls, void *parameter);

static void input(StateStack *state_stack);

static void update();

// -----------------------------------------------------------------------------
// Public variable definitions
// -----------------------------------------------------------------------------
State race_single_state = {
        &initialize,
        &update,
        &input
};

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void initialize(StateType ls, void *parameter)
{
    // Disable display until we're ready
    REG_DISPCNT = 0;

    leaving_state = ls;

    if (leaving_state == RACECAR_SELECT)
    {
        selected_car = *((int *) parameter);
    }
}

void update()
{

}

void input(StateStack *state_stack)
{
    switch (leaving_state)
    {
        case NONE:
            push_state(state_stack, &racecar_select_state, RACE_SINGLE, NULL);
            break;
        case RACECAR_SELECT:
            push_state(state_stack, &race_state, RACE_SINGLE, &selected_car);
            break;
        default:
            pop_state(state_stack, RACE_SINGLE, NULL);
    }
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------