#include "timer.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void load_timer(Timer *timer, OBJ_ATTR *oam, int x, int y)
{
    timer->oam = oam;
    timer->frames = 0;
    timer->millis = 0;
    timer->seconds = 0;
    timer->minutes = 0;
    // There are five sprites in the timer: "mm:ss"
    for (int i = 0; i < 5; i++)
    {
        oam[i].attr0 = ATTR0_4BPP | ATTR0_SQUARE | y;
        oam[i].attr1 = ATTR1_SIZE_8x8 | (i * 8 + x);
        oam[i].attr2 = ATTR2_PALBANK(4) | 80; // "0"
    }
    oam[2].attr2 = ATTR2_PALBANK(4) | 90;
}

void update_timer(Timer *timer)
{
    timer->millis++;
    timer->frames++;
    if (timer->millis == 60)
    {
        timer->millis = 0;
        timer->seconds++;
        if (timer->seconds == 60)
        {
            timer->seconds = 0;
            timer->minutes++;
        }
    }
    timer->oam[0].attr2 = ATTR2_PALBANK(4) | (80 + timer->minutes / 10);
    timer->oam[1].attr2 = ATTR2_PALBANK(4) | (80 + timer->minutes % 10);
    timer->oam[3].attr2 = ATTR2_PALBANK(4) | (80 + timer->seconds / 10);
    timer->oam[4].attr2 = ATTR2_PALBANK(4) | (80 + timer->seconds % 10);
}


// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------