#include <tonc.h>
#include "title.h"
#include "state.h"
// Data
#include "titlescreen.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------
static void initialize();

static void update();

static void input();


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

static void initialize()
{
    REG_DISPCNT = DCNT_MODE4 | DCNT_BG2;

    memcpy32(vid_mem, titlescreenBitmap, titlescreenBitmapLen / 4);
    memcpy32(pal_bg_mem, titlescreenPal, titlescreenPalLen / 4);
}

static void update()
{

}

static void input()
{
    if (key_hit(KEY_START)) return;
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------