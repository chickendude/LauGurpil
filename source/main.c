#include "race.h"
#include "state.h"
#include "title.h"

// TODO: Complete race (remaining AI cars) after player finishes
//  - Show user's stats while race completes
//  - Probably want to do this very quickly in a for loop without waiting for
//    VBlank (and perhaps set a timeout)
// TODO: Add final stop for cars that have finished all their laps
// TODO: Determine place during race time:
//  a) Add "checkpoints" to game to determine player/car's progress
//  b) Calculate which checkpoint each car is at and sort based on that
//  c) If two cars are at the same checkpoint, calculate distance to the next
//     checkpoint and determine place based on who's closest
//     * Alternatively, use which car reached the checkpoint first
// TODO: Add collisions between cars
// TODO: Add counter to main update and add to a "global.c/h" file to use as a
//       random number. Use to calculate how accurately ai cars should follow
//       the paths.
// PRE RACE -----------
// TODO: Add locked state to cars to enable unlocking new vehicles


// 5, 5
//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void)
{
//---------------------------------------------------------------------------------
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
