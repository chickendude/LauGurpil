#include "state.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void push_state(StateStack *state_stack, State *state, void *parameter)
{
    state_stack->index++;

    int i = state_stack->index;
    state_stack->states[i] = state;
    state_stack->states[i]->initialize(parameter);
}

void pop_state(StateStack *state_stack, void *parameter)
{
    state_stack->index--;

    int i = state_stack->index;
    state_stack->states[i]->initialize(parameter);
}


// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------