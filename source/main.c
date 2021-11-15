#include "race.h"
#include "state.h"
#include "title.h"

// TODO: Add function to switch camera (probably need to redraw full tilemap)
// TODO: Show data of currently followed car (speed, lap time, etc.)
// TODO: Complete race (remaining AI cars) after player finishes
//  - Probably want to do this very quickly in a for loop without waiting for
//    VBlank (and perhaps set a timeout)
// TODO: Display AI car times in minutes/seconds
// TODO: Add final stop for cars that have finished all their laps
// TODO: Determine place during race time:
//  a) Add "checkpoints" to game to determine player/car's progress
//  b) Calculate which checkpoint each car is at and sort based on that
//  c) If two cars are at the same checkpoint, calculate distance to the next
//     checkpoint and determine place based on who's closest
//     * Alternatively, use which car reached the checkpoint first
// TODO: Add collisions between cars
// PRE RACE -----------
// TODO: Add locked state to cars to enable unlocking new vehicles


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
