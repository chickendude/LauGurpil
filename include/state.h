#ifndef TEST_STATE_H
#define TEST_STATE_H

#include "types.h"

struct State
{
    void (*initialize)(void *);
    void (*update)();
    void (*input)(StateStack *);
};

struct StateStack
{
    int index;
    State *states[10];
};

void push_state(StateStack *state_stack, State *state, void *parameter);

void pop_state(StateStack *state_stack, void *parameter);

#endif //TEST_STATE_H
