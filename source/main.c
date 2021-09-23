#include "tonc.h"
// sprites
#include "cars.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE *) obj_buffer;

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void)
{
//---------------------------------------------------------------------------------
    REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;
    oam_init(obj_buffer, 128);

    memcpy32(tile_mem[4], carsTiles, carsTilesLen / 4);
    memcpy32(pal_obj_mem, carsPal, carsPalLen / 4);


    obj_set_attr(obj_buffer,
                 ATTR0_SQUARE | ATTR0_4BPP | ATTR0_AFF | ATTR0_AFF_DBL_BIT,
                 ATTR1_SIZE_16x16 | ATTR1_AFF_ID(0),
                 ATTR2_PALBANK(0) | ATTR2_ID(0));

    obj_aff_identity(&obj_aff_buffer[0]);
    obj_set_pos(&obj_buffer[0], 0, 0);
    obj_aff_rotate(&obj_aff_buffer[0], 0x1800);
    oam_copy(oam_mem, obj_buffer, 128);

    int i = 0;
    int angle = 0;
    int dx, dy;
    int speed = 0;
    signed int x, y;
    x = y = 80;
    while (i >= 0)
    {
        vid_vsync();
        oam_copy(oam_mem, obj_buffer, 128);

        key_poll();
        angle -= key_tri_horz() * 0x0200;
        speed = key_tri_vert();
        dx = speed * lu_sin(angle);
        dy = speed * lu_cos(angle);
        x += dx;
        y += dy;

        obj_set_pos(&obj_buffer[0], x >> 12, y >> 12);
        obj_aff_identity(&obj_aff_buffer[0]);
        obj_aff_rotate(&obj_aff_buffer[0], angle);
    }

    return 0;
}
