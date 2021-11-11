#include <tonc.h>
#include "tournament.h"
#include "state.h"
#include "text.h"
#include "track.h"

const Tournament tournament_1 = {
        "IKASTEN\0",
        {&track_1, &track_2, &track_3, &track_4},
};

const Tournament tournament_2 = {
        "IRAKASTEN\0",
        {&track_2, &track_3, &track_4, &track_1},
};

const Tournament *tournaments[NUM_TOURNAMENTS] = {&tournament_1, &tournament_2};

#define TITLE_X 5
#define TITLE_Y 4

static StateType prev_state;
static int counter = 20;
static int direction = 1;
static int selected_tournament;

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------
static void initialize(StateType leaving_state, void *parameter);

static void input(StateStack *state_stack);

static void update();

static void highlight_tournament();

// -----------------------------------------------------------------------------
// Public variable definitions
// -----------------------------------------------------------------------------
State tournament_select_state = {
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

    // Set background color
    pal_bg_mem[0] = CLR_GRAY;

    prepare_text(3, 29);
    print_text(se_mem[29], 4, 1, "SELECT A TOURNAMENT:");
    for (int i = 0; i < NUM_TOURNAMENTS; i++)
    {
        const char *title = tournaments[i]->title;
        print_text(se_mem[29], TITLE_X, TITLE_Y + (i * 2), title);
    }

    selected_tournament = 0;
    highlight_tournament();

    REG_DISPCNT = DCNT_MODE0 | DCNT_BG3;
}

void update()
{
    int clr = counter >> 2;
    pal_bg_mem[5] = RGB15_SAFE(clr, clr, clr);
    counter += direction;
    if (counter == 40 || counter == 20) direction *= -1;
}

void input(StateStack *state_stack)
{
    if (key_hit(KEY_B))
    {
        pop_state(state_stack, TOURNAMENT_SELECT, NULL);
    } else if (key_hit(KEY_A)) {
        pop_state(state_stack, TOURNAMENT_SELECT, tournaments[selected_tournament]);
    } else if (key_hit(KEY_UP))
    {
        selected_tournament--;
        if (selected_tournament < 0) selected_tournament = NUM_TOURNAMENTS - 1;
        highlight_tournament();
    } else if (key_hit(KEY_DOWN))
    {
        selected_tournament++;
        if (selected_tournament == NUM_TOURNAMENTS) selected_tournament = 0;
        highlight_tournament();
    }
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------
void highlight_tournament()
{
    // Update the palette for all letters in the title
    for (int t_id = 0; t_id < NUM_TOURNAMENTS; t_id++)
    {
        // Use palette 0 if the current tournament is selected
        int pal_bank = t_id == selected_tournament ? 0 : 15;
        for (int i = 0; i < 10; i++)
        {
            int sb = 32 * (TITLE_Y + t_id * 2) + TITLE_X + i;
            int tile_id = se_mem[29][sb] & ~SE_PALBANK_MASK;
            se_mem[29][sb] = tile_id | SE_PALBANK(pal_bank);
        }
    }
}
