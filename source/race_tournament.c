#include <tonc.h>
#include "race_tournament.h"
#include "race.h"
#include "racecar_select.h"
#include "state.h"
#include "tournament.h"

static RaceData race_data;
static Tournament *tournament;
static int track_index;

static StateType prev_state;

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

static void initialize(StateType leaving_state, void *parameter);

static void input(StateStack *state_stack);

static void update();

// -----------------------------------------------------------------------------
// Public variable definitions
// -----------------------------------------------------------------------------
State race_tournament_state = {
        &initialize,
        &update,
        &input
};

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void initialize(StateType leaving_state, void *parameter)
{
    // Disable display until we're ready
    REG_DISPCNT = 0;

    prev_state = leaving_state;

    if (prev_state == TOURNAMENT_SELECT && parameter != NULL)
    {
        tournament = (Tournament *) parameter;
        track_index = 0;
    } else if (prev_state == RACECAR_SELECT && parameter != NULL)
    {
        race_data.car_data = (RacecarData *) parameter;
    }
}

void update()
{

}

void input(StateStack *state_stack)
{
    switch (prev_state)
    {
        case NONE:
            push_state(state_stack, &tournament_select_state, RACE_TOURNAMENT,
                       NULL);
            break;
        case TOURNAMENT_SELECT:
            push_state(state_stack, &racecar_select_state, RACE_TOURNAMENT,
                       NULL);
            break;
        case RACECAR_SELECT:
        case RACE_STATS:
            if (track_index < 4)
            {
                race_data.track = tournament->tracks[track_index++];
                push_state(state_stack, &race_state, RACE_TOURNAMENT,
                           &race_data);
            } else pop_state(state_stack, RACE_TOURNAMENT, NULL);
            break;
        default:
            pop_state(state_stack, RACE_TOURNAMENT, NULL);
    }
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------