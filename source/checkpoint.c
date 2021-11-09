#include "checkpoint.h"
#include "racecar.h"
#include "track.h"

const Checkpoint t1_checkpoints[] = {
        {101, 49,  50,  true}, // 1
        {35,  85,  50,  true},
        {29,  140, 30,  true},
        {112, 152, 70,  true},
        {248, 144, 200, true},
        {436, 156, 200, true}, // 6
        {538, 265, 50,  true},
        {507, 349, 50,  true},
        {338, 344, 50,  true},
        {268, 281, 30,  true}, // 10
        {139, 266, 40,  true}, // 11
        {111, 340, 40,  false}, // 12
        {144, 430, 200, true}, // 13
        {308, 475, 200, true}, // 14
        {508, 455, 200, true}, // 14
        {712, 345, 200, true}, // 15
        {739, 183, 200, true}, // 16
        {718, 69,  40,  true}, // 17
        {294, 32,  200, true}, // 18
};

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

Checkpoint* check_checkpoint(const Track *track, Racecar *car)
{
    Checkpoint *checkpoint = &track->checkpoints[car->checkpoint_index];
    int x = car->x >> 12;
    int y = car->y >> 12;
    int check_x = checkpoint->x - 16;
    int check_y = checkpoint->y - 16;

    // Advance to next checkpoint if car is near current checkpoint
    if (x >= check_x && x <= check_x + 32 &&
        y >= check_y && y <= check_y + 32)
    {
        car->checkpoint_index =
                (car->checkpoint_index + 1) % track->num_checkpoints;
    }
    return checkpoint;
}


// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------