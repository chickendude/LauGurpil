#include "checkpoint.h"
#include "racecar.h"
#include "track.h"
#include "track1_ai_checkpoints.h"

//
// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

const Checkpoint *check_checkpoint(const Track *track, Racecar *car)
{
    const Checkpoint *checkpoint = &track->checkpoints[car->checkpoint_index];
    int x = car->x >> 12;
    int y = car->y >> 12;
    int check_x = checkpoint->x - 16;
    int check_y = checkpoint->y - 16;

    // Advance to next checkpoint if car is near current checkpoint
    if ((x >= check_x && x <= check_x + 32 && checkpoint->direction == VERT) ||
        (y >= check_y && y <= check_y + 32 && checkpoint->direction == HORZ))
    {
        car->checkpoint_index =
                (car->checkpoint_index + 1) % track->num_checkpoints;
    }
    return checkpoint;
}

void check_progress_marker(const Track *track, Racecar *car)
{
    const Checkpoint *marker = &track->progress_markers[car->progress_index];
    int x = car->x >> 12;
    int y = car->y >> 12;
    int mark_x = marker->x - 16;
    int mark_y = marker->y - 16;

    // Advance to next checkpoint if car is near current checkpoint
    if ((x >= mark_x && x <= mark_x + 32 && marker->direction == VERT) ||
        (y >= mark_y && y <= mark_y + 32 && marker->direction == HORZ))
    {
        car->progress_index++;
    }
}


// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------