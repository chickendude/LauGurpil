#include <tonc.h>
#include "racecar_select.h"
#include "state.h"
// sprites
#include "cars.h"

int angle;

int selected_car_index;

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

static void initialize(void *);

static void input(StateStack *state_stack);

static void update();

// -----------------------------------------------------------------------------
// Public variable definitions
// -----------------------------------------------------------------------------
State racecar_select_state = {
        &initialize,
        &update,
        &input
};

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void initialize(void *_)
{
    angle = 0;
    selected_car_index = 0;
    // Disable display until we're ready
    REG_DISPCNT = 0;
    oam_init(obj_mem, 128);

    // Load Car sprites
    memcpy32(tile_mem[4], carsTiles, carsTilesLen / 4);
    memcpy32(pal_obj_mem, carsPal, carsPalLen / 4);
    pal_bg_mem[0] = CLR_GRAY;

    for (int i = 0; i < 7; i++)
    {
        obj_set_attr(&obj_mem[i],
                     ATTR0_4BPP| ATTR0_SQUARE | ATTR0_AFF | ATTR0_AFF_DBL_BIT | 10,
                     ATTR1_SIZE_16x16 | ATTR1_AFF_ID(0) | (10 + 30 * i),
                     ATTR2_PALBANK(i) | i * 4
                );
    }

    REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;
}

void update()
{
    angle += 0x0250;
    for (int i = 0; i < 2; i++)
    {
        obj_aff_rotate(&obj_aff_mem[0], angle);
    }
}

void input(StateStack *state_stack)
{
    u16 *attr1 = &obj_mem[selected_car_index].attr1;
    *attr1 = (*attr1 & ~ATTR1_AFF_ID_MASK) | ATTR1_AFF_ID(0);
    if (key_hit(KEY_RIGHT)) {
        selected_car_index++;
        if (selected_car_index == 7) selected_car_index = 0;
    }
    else if (key_hit(KEY_LEFT)) {
        selected_car_index--;
        if (selected_car_index < 0) selected_car_index = 7;
    }

    attr1 = &obj_mem[selected_car_index].attr1;
    *attr1 = (*attr1 & ~ATTR1_AFF_ID_MASK) | ATTR1_AFF_ID(1);

    if (key_hit(KEY_A)) {
        pop_state(state_stack, &selected_car_index);
    }
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------