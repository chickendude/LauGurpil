#include <tonc.h>
#include <malloc.h>
#include "racecar_select.h"
#include "constants.h"
#include "racecar.h"
#include "state.h"
#include "text.h"
// sprites
#include "cars.h"

int angle;

int selected_car_index;

const RacecarData **selected_cars;

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

static void initialize(StateType, void *);

static void input(StateStack *state_stack);

static void update();

static void update_stats();

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
void initialize(StateType _ls, void *_)
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

    prepare_text(3, 29);

    for (int i = 0; i < NUM_CARS; i++)
    {
        obj_set_attr(&obj_mem[i],
                     ATTR0_4BPP | ATTR0_SQUARE | ATTR0_AFF | ATTR0_AFF_DBL_BIT |
                     10,
                     ATTR1_SIZE_16x16 | ATTR1_AFF_ID(0) | (10 + 30 * i),
                     ATTR2_PALBANK(i) | i * 4
        );
    }

    print_text(se_mem[29], 7, 10, "STATS:");
    print_text(se_mem[29], 7, 11, "SPD:<");
    print_text(se_mem[29], 7, 12, "ACC:<");
    print_text(se_mem[29], 7, 13, "TRN:<");
    update_stats();

    REG_DISPCNT = DCNT_MODE0 | DCNT_BG3 | DCNT_OBJ | DCNT_OBJ_1D;
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
    if (key_hit(KEY_RIGHT))
    {
        selected_car_index++;
        if (selected_car_index == NUM_CARS) selected_car_index = 0;
        update_stats();
    } else if (key_hit(KEY_LEFT))
    {
        selected_car_index--;
        if (selected_car_index < 0) selected_car_index = NUM_CARS - 1;
        update_stats();
    }

    attr1 = &obj_mem[selected_car_index].attr1;
    *attr1 = (*attr1 & ~ATTR1_AFF_ID_MASK) | ATTR1_AFF_ID(1);
    obj_aff_scale(&obj_aff_mem[1], 0xB0, 0xB0);

    if (key_hit(KEY_B))
    {
        // Screen was cancelled
        pop_state(state_stack, RACECAR_SELECT, NULL);
    } else if (key_hit(KEY_A))
    {
        // Car was selected
        if (selected_cars == NULL)
        {
            selected_cars = malloc(sizeof *selected_cars * (NUM_AI_CARS + 1));
        }

        // First car is player's car
        selected_cars[0] = cars[selected_car_index];
        // Select the AI cars
        for (int i = 1; i < 5; i++)
        {
            int index = (selected_car_index + i) % NUM_CARS;
            selected_cars[i] = cars[index];
        }
        pop_state(state_stack, RACECAR_SELECT, selected_cars);
    }
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------

void update_stats()
{
    char spd_txt[] = "          ";
    char acc_txt[] = "          ";
    char trn_txt[] = "          ";
    const RacecarData *data = cars[selected_car_index];
    int spd = data->max_speed;
    int acc = data->acceleration_power;
    int trn = data->turning_power;
    for (int i = 0; i < 9; i++)
    {
        if (spd > i + 2) spd_txt[i] = '=';
        else if (spd == i + 2) spd_txt[i] = '>';
        if (acc > i + 2) acc_txt[i] = '=';
        else if (acc == i + 2) acc_txt[i] = '>';
        if (trn > i + 2) trn_txt[i] = '=';
        else if (trn == i + 2) trn_txt[i] = '>';
    }
    print_text(se_mem[29], 12, 8, data->name);
    print_text(se_mem[29], 12, 11, spd_txt);
    print_text(se_mem[29], 12, 12, acc_txt);
    print_text(se_mem[29], 12, 13, trn_txt);
    print_number(se_mem[29], 21, 11, spd);
    print_number(se_mem[29], 21, 12, acc);
    print_number(se_mem[29], 21, 13, trn);
}