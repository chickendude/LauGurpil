#ifndef TEST_STATE_H
#define TEST_STATE_H

#include "types.h"

struct State
{
    void (*initialize)();
    void (*update)();
    void (*input)(StateStack *);
};

struct StateStack
{
    int index;
    State *states[10];
};
#endif //TEST_STATE_H
