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

/**
 * Pushes a new state onto the state stack.
 *
 * You can optionally provide a parameter as a pointer which will be passed to
 * the new state.
 *
 * @param state_stack The stack to push the state onto.
 * @param state The state to push onto the stack.
 * @param parameter A pointer to some data to share between the two states.
 */
void push_state(StateStack *state_stack, State *state, void *parameter);

/**
 * Pop the current state off the stack and load the previous state.
 *
 * If need be, you can pass a pointer back to the previous state.
 *
 * @param state_stack The stack to push the state onto.
 * @param parameter A pointer to some data to pass back to previous stack item.
 */
void pop_state(StateStack *state_stack, void *parameter);

#endif //TEST_STATE_H
