#include "race.h"
#include "state.h"
#include "title.h"

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void)
{
//---------------------------------------------------------------------------------
    State cur_state = title_state;

    cur_state.initialize();
//    initialize();

    int i = 0;
    while (i >= 0)
    {
        cur_state.input();
    }

    return 0;
}
