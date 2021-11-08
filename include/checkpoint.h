#ifndef LAUGURPIL_CHECKPOINT_H
#define LAUGURPIL_CHECKPOINT_H

#include <tonc.h>
#include "types.h"

struct Checkpoint
{
   int x, y;
   int speed;
   bool is_vertical;
};

#define T1_CHECKPOINT_COUNT 18
extern const Checkpoint t1_checkpoints[T1_CHECKPOINT_COUNT];

#endif //LAUGURPIL_CHECKPOINT_H
