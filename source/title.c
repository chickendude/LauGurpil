#include <tonc.h>
#include "title.h"
#include "race_single.h"
#include "state.h"
// Data
#include "titlescreen.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------
static void initialize(StateType, void *);

static void update();

static void input(StateStack *state_stack);


// -----------------------------------------------------------------------------
// Public variable definitions
// -----------------------------------------------------------------------------
State title_state = {
        &initialize,
        &update,
        &input
};

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

static void initialize(StateType prev_state, void *parameter)
{
    REG_DISPCNT = DCNT_MODE4 | DCNT_BG2;

    memcpy32(vid_mem, titlescreenBitmap, titlescreenBitmapLen / 4);
    memcpy32(pal_bg_mem, titlescreenPal, titlescreenPalLen / 4);
}

static void update()
{

}

static void input(StateStack *state_stack)
{
    if (key_hit(KEY_START))
    {
        push_state(state_stack, &race_single_state, NONE, NULL);
    }
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------