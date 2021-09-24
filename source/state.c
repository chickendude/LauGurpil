#include "state.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void push_state(StateStack *state_stack, State *state)
{
    state_stack->index++;

    int i = state_stack->index;
    state_stack->states[i] = state;
    state_stack->states[i]->initialize();
}

void pop_state(StateStack *state_stack)
{
    state_stack->index--;

    int i = state_stack->index;
    state_stack->states[i]->initialize();
}


// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------