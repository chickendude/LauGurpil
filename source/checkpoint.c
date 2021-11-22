#include "checkpoint.h"
#include "racecar.h"
#include "track.h"

const Checkpoint t1_checkpoints[] = {
        {101, 49,  200, VERT}, // 1
        {45,  85,  80,  HORZ}, // 2
        {29,  140, 30,  HORZ}, // 3
        {112, 152, 200, VERT}, // 4
        {248, 144, 200, VERT}, // 5
        {436, 156, 200, VERT}, // 6
        {538, 265, 200, HORZ}, // 7
        {507, 349, 200, VERT}, // 8
        {338, 344, 200, VERT}, // 9
        {268, 281, 30,  HORZ}, // 10
        {139, 266, 40,  VERT}, // 11
        {111, 340, 40,  HORZ}, // 12
        {144, 430, 200, VERT}, // 13
        {308, 475, 200, VERT}, // 14
        {508, 455, 200, VERT}, // 14
        {712, 345, 200, HORZ}, // 15
        {739, 183, 200, HORZ}, // 16
        {718, 69,  40,  HORZ}, // 17
        {294, 32,  200, VERT}, // 18
};

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