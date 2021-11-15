#ifndef LAUGURPIL_CHECKPOINT_H
#define LAUGURPIL_CHECKPOINT_H

#include <tonc.h>
#include "types.h"

typedef enum { VERT, HORZ } CHECKPOINT_DIRECTION;
struct Checkpoint
{
   int x, y;
   int speed;
   CHECKPOINT_DIRECTION direction;
};

#define T1_CHECKPOINT_COUNT 19
extern const Checkpoint t1_checkpoints[T1_CHECKPOINT_COUNT];

Checkpoint* check_checkpoint(const Track *track, Racecar *car);

#endif //LAUGURPIL_CHECKPOINT_H
