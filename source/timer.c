#include "timer.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void load_timer(Timer *timer)
{
    timer->frames = 0;
    timer->millis = 0;
    timer->seconds = 0;
    timer->minutes = 0;
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
}


// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------