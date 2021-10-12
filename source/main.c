#include "race.h"
#include "state.h"
#include "title.h"

// RACE -------------
// TODO: Return to menu when completed
// TODO: Record lap times
// TODO: Show race stats
// PRE RACE -----------
// TODO: Car selection
// TODO: Car stats (max speed, acceleration, turning)
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

    cur_state->initialize(NULL);

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
