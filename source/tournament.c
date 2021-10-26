#include "tournament.h"
#include "track.h"

Tournament* tournaments[NUM_TOURNAMENTS] = {&tournament_1, &tournament_2};

const Tournament tournament_1 = {
        "Ikasten\n",
        {&track_1, &track_2, &track_3, &track_4},
};

const Tournament tournament_2 = {
        "Ikasten\n",
        {&track_1, &track_2, &track_3, &track_4},
};
// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------