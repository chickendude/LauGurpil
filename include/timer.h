#ifndef LAUGURPIL_TIMER_H
#define LAUGURPIL_TIMER_H

#include <tonc.h>
#include "types.h"

struct Timer
{
    /**
     * There are ~60 FPS, so when millis == 60 we increment seconds and reset
     * millis to 0.
     */
    int millis;
    int seconds, minutes;
    int frames;
    OBJ_ATTR *oam;
};

void load_timer(Timer *timer, OBJ_ATTR *oam, int x, int y);

void update_timer(Timer *timer);

#endif //LAUGURPIL_TIMER_H
