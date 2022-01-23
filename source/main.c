#include <tonc.h>
#include <maxmod.h>
#include "race.h"
#include "state.h"
#include "title.h"

#include "soundbank.h"
#include "soundbank_bin.h"

// TODO: Add timeout when automatically finishing a race
// TODO: Add final stop for cars that have finished all their laps
// TODO: Look into why it AI cars seem to shake (perhaps camera race issue
//       between player and AI cars)
// PRE RACE -----------
// TODO: Add locked state to cars to enable unlocking new vehicles


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

    // Set up interrupts for maxmod
    irq_init(isr_master);
    irq_add(II_VBLANK, mmVBlank);
    irq_enable(II_VBLANK);


    // initialize maxmod
    mmInitDefault((mm_addr) soundbank_bin, 4);
    mmSetModuleVolume(500);
    mmStart(MOD_CHIPTUNES, MM_PLAY_LOOP);

    int i = 0;
    while (i >= 0)
    {
        cur_state = state_stack.states[state_stack.index];

        mmFrame();
        vid_vsync();
        cur_state->update();

        key_poll();
        cur_state->input(&state_stack);
    }

    return 0;
}
