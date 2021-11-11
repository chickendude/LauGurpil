#ifndef LAUGURPIL_TOURNAMENT_H
#define LAUGURPIL_TOURNAMENT_H

#include "constants.h"
#include "types.h"

struct Tournament
{
    const char title[10];
    const Track *tracks[4];
};

extern const Tournament* tournaments[NUM_TOURNAMENTS];
extern const Tournament tournament_1, tournament_2;

extern State tournament_select_state;

#endif //LAUGURPIL_TOURNAMENT_H
