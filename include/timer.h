#ifndef LAUGURPIL_TIMER_H
#define LAUGURPIL_TIMER_H

#include "types.h"

struct Timer
{
    /**
     * There are ~60 FPS, so when frames == 60 we increment seconds and reset
     * frames to 0.
     */
    int frames;
    int seconds, minutes;
};

void reset_timer(Timer *timer);

void update_timer(Timer *timer);

#endif //LAUGURPIL_TIMER_H
