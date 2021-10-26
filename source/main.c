#include "race.h"
#include "state.h"
#include "title.h"

// PRE RACE -----------
// TODO: Add locked state to cars to enable unlocking new vehicles
// TODO: Set up tournament state
//  #1. Create files
//  #2. Make car/track selection independent of each other
//  3. Create tournament struct
//  4. Create tournament selection state
//  5. Select car and load track

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
