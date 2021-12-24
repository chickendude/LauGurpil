#include "race.h"
#include "state.h"
#include "title.h"

// TODO: Add timeout when automatically finishing a race
// TODO: Stop updating place marker when player has finished
// TODO: Add final stop for cars that have finished all their laps
// TODO: Add collisions between cars
// TODO: Add counter to main update and add to a "global.c/h" file to use as a
//       random number. Use to calculate how accurately ai cars should follow
//       the paths.
// PRE RACE -----------
// TODO: Add locked state to cars to enable unlocking new vehicles


// 5, 5
//------------------------------------------------------------------------------
// Program entry point
//------------------------------------------------------------------------------
int main(void)
{
//------------------------------------------------------------------------------
    StateStack state_stack;
    state_stack.index = 0;
    state_stack.states[0] = &title_state;

    State *cur_state = state_stack.states[state_stack.index];

    cur_state->initialize(NONE, NULL);

    int i = 0;
    while (i >= 0)
    {
        cur_state = state_stack.states[state_stack.index];

        vid_vsync();
        cur_state->update();

        key_poll();
        cur_state->input(&state_stack);
    }

    return 0;
}
