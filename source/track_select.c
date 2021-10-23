#include <tonc.h>
#include "track_select.h"
#include "race.h"
#include "state.h"
#include "text.h"
#include "track.h"

static RaceData race_data;

int selected_track_index;

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

static void initialize(StateType, void *);

static void input(StateStack *state_stack);

static void update();

static void build_track_sprites();

static int grab_tile_id(int x, int y);

// -----------------------------------------------------------------------------
// Public variable definitions
// -----------------------------------------------------------------------------
State track_select_state = {
        &initialize,
        &update,
        &input
};

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void initialize(StateType _prev_state, void *parameter)
{
    // Save car passed in from [racecar_select]
    race_data.car_data = (RacecarData *) parameter;
    race_data.track = tracks[0];

    selected_track_index = 0;

    // Disable display until we're ready
    REG_DISPCNT = 0;
    oam_init(obj_mem, 128);

    // Load Car sprites
    build_track_sprites();
    pal_obj_mem[0] = CLR_WHITE;
    pal_obj_mem[1] = RGB15(16, 16, 16);
    pal_obj_mem[2] = CLR_BLACK;
    pal_obj_mem[3] = CLR_WHITE;

    // Set background color
    pal_bg_mem[0] = RGB15(20, 20, 20);

    prepare_text(3, 31);

    obj_set_attr(obj_mem,
                 ATTR0_4BPP | ATTR0_SQUARE | 90,
                 ATTR1_SIZE_64x64 | 100,
                 ATTR2_PALBANK(0) | 0
    );

    print_text(se_mem[31], 10, 2, "SELECT TRACK");

    REG_DISPCNT = DCNT_MODE0 | DCNT_BG3 | DCNT_OBJ | DCNT_OBJ_1D;
}

void update()
{
}

void input(StateStack *state_stack)
{
    if (key_hit(KEY_RIGHT))
    {
        selected_track_index++;
        if (selected_track_index == NUM_TRACKS) selected_track_index = 0;
        race_data.track = tracks[selected_track_index];
        build_track_sprites();
    } else if (key_hit(KEY_LEFT))
    {
        selected_track_index--;
        if (selected_track_index < 0) selected_track_index = NUM_TRACKS - 1;
        race_data.track = tracks[selected_track_index];
        build_track_sprites();
    }

    if (key_hit(KEY_B))
    {
        pop_state(state_stack, TRACK_SELECT, race_data.car_data);
    } else if (key_hit(KEY_A))
    {
        pop_state(state_stack, TRACK_SELECT, NULL);
        pop_state(state_stack, TRACK_SELECT, &race_data);
    }
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------
static int grab_tile_id(const int x, const int y)
{
    const Track *track = race_data.track;

    int id = track->tilemap[y * track->width + x];
    if (id == 0 || id == 64) return 0;
    if (id == 9 || id == 2 || id == 17 || (id >= 22 && id <= 24)) return 3;
    return 1;
}

static void build_track_sprites()
{
    const Track *track = race_data.track;

    TILE4 clear_tile = {{0x00000000, 0x00000000, 0x00000000, 0x00000000,
                                0x00000000, 0x00000000, 0x00000000, 0x00000000}};

    // Clear out tile memory data in case a track is less than 64x64 tiles
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            tile_mem[4][i * 8 + j] = clear_tile;
        }
    }

    // Go through the tiles 8 at a time and pack them into a 4bpp sprite (one
    // pixel per tile)
    for (int y = 0; y < track->height; y += 2)
    {
        for (int x = 0; x < track->width; x += 16)
        {
            int t1 = grab_tile_id(x, y);
            int t2 = grab_tile_id(x + 2, y);
            int t3 = grab_tile_id(x + 4, y);
            int t4 = grab_tile_id(x + 6, y);
            int t5 = grab_tile_id(x + 8, y);
            int t6 = grab_tile_id(x + 10, y);
            int t7 = grab_tile_id(x + 12, y);
            int t8 = grab_tile_id(x + 14, y);
            int pixel = (t8 << 28) + (t7 << 24) + (t6 << 20) + (t5 << 16) +
                        (t4 << 12) + (t3 << 8) + (t2 << 4) + t1;
            int cols_left = track->width - x;
            // Clear out remaining last frame if width not divisible by 16
            if (cols_left < 16) pixel &= (-1 ^ (-1 << (cols_left * 2)));
            tile_mem[4][(y / 16) * 8 + (x / 16)].data[(y / 2) % 8] = pixel;
        }
    }

    // Center the sprite horizontally based on the width of the track
    int x = SCREEN_WIDTH / 2 - track->width / 4;
    obj_mem[0].attr1 = ATTR1_SIZE_64x64 | x;
}