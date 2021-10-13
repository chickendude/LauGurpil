#ifndef TEST_STATE_H
#define TEST_STATE_H

#include "types.h"

typedef enum StateType
{
    NONE,
    RACE,
    RACE_SINGLE,
    RACE_STATS,
    RACE_TOURNAMENT,
    RACECAR_SELECT
} StateType;

struct State
{
    void (*initialize)(StateType, void *);

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
 * @param cur_state The current state, the one putting the new state on the
 * stack.
 * @param parameter A pointer to some data to share between the two states.
 */
void push_state(StateStack *state_stack, State *state, StateType cur_state,
                void *parameter);

/**
 * Pop the current state off the stack and load the previous state.
 *
 * If need be, you can pass a pointer back to the previous state.
 *
 * @param state_stack The stack to push the state onto.
 * @param leaving_state The current state that is being popped off the stack.
 * @param parameter A pointer to some data to pass back to previous stack item.
 */
void
pop_state(StateStack *state_stack, StateType leaving_state, void *parameter);

#endif //TEST_STATE_H
