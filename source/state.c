#include "state.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void push_state(StateStack *state_stack, State *state, StateType cur_state,
                void *parameter)
{
    state_stack->index++;

    int i = state_stack->index;
    state_stack->states[i] = state;
    state_stack->states[i]->initialize(cur_state, parameter);
}

void
pop_state(StateStack *state_stack, StateType prev_state, void *parameter)
{
    state_stack->index--;

    int i = state_stack->index;
    state_stack->states[i]->initialize(prev_state, parameter);
}


// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------