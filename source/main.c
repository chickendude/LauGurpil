#include "race.h"
#include "state.h"
#include "title.h"

// PRE RACE -----------
// TODO: Show car stats on selection screen (maybe switch to point system:
//  max_speed = 1-10, acceleration = 1-10, etc. where each point = 5mph
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
