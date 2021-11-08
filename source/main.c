#include "race.h"
#include "state.h"
#include "title.h"

// TODO: Copy code that tests if car is in checkpoint to checkpoint.c file
// TODO: Keep track of ai cars' laps
// TODO: Add function to switch camera (probably need to redraw full tilemap)
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
