#include <tonc.h>
#include "race_single.h"
#include "race.h"
#include "racecar_select.h"
#include "state.h"

int selected_car;

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

static void initialize(void *parameter);

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
void initialize(void *parameter)
{
    // Disable display until we're ready
    REG_DISPCNT = 0;

    if (parameter != NULL) {
        selected_car = *((int*)parameter);
    } else {
        selected_car = -1;
    }
}

void update()
{

}

void input(StateStack *state_stack)
{
    if (selected_car < 0) {
        push_state(state_stack, &racecar_select_state, NULL);
    } else {
        push_state(state_stack, &race_state, &selected_car);
    }
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------