#include "race.h"
#include "state.h"
#include "title.h"

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

    cur_state->initialize();

    int i = 0;
    while (i >= 0)
    {
        key_poll();
        State *cur_state = state_stack.states[state_stack.index];
        cur_state->input(&state_stack);
    }

    return 0;
}
