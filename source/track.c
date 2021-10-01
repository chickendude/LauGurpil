#include <tonc.h>
#include "track.h"
#include "constants.h"
#include "race.h"
// sprite data
#include "ground_tiles.h"
#include "track_tiles.h"
// tilemap data
#include "tracks.h"


const Track track_1 = {
        18, 1,
        0x8000,
        track1Width, track1Height,
        track1_tilemap
};
// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

void draw_tile(int x, int y, int tile_offset, const Track *track);

bool is_tile_in_map(int x, int y, const Track *track);

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

void load_track(const Track *track, Camera *camera)
{
    // CBB = charblock base, where the tile sprites are stored. there are 4 CBB
    // SBB = screenblock base, where the tilemap is stored. there are 32 SBBs
    REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_REG_32x32 | BG_4BPP;
    REG_BG1CNT = BG_CBB(1) | BG_SBB(31) | BG_REG_32x32 | BG_4BPP;
    memcpy32(tile_mem, track_tilesTiles, track_tilesTilesLen / 4);
    memcpy32(&pal_bg_mem[2 * 16], track_tilesPal, 16);

    memcpy32(tile_mem[1], ground_tilesTiles, ground_tilesTilesLen / 4);
    memcpy32(&pal_bg_mem[3 * 16], ground_tilesPal, 16);

    camera->x = (track->start_x << 4) - SCREEN_WIDTH / 2;
    camera->y = (track->start_y << 4) - SCREEN_HEIGHT / 2;

    int x, y;
    int bg_tiles_offset = track->width * track->height;
    int start_x = camera->x >> 4;
    int start_y = camera->y >> 4;
    for (y = start_y; y < BG_HEIGHT_16 + start_y; y++)
    {
        for (x = start_x; x < BG_WIDTH_16 + start_x; x++)
        {
            draw_tile(x, y, bg_tiles_offset, track);
        }
    }
}

void update_tilemap(Race *race)
{
    Camera cam = race->camera;
    Camera prev_cam = race->prev_camera;
    int bg_tiles_offset = race->track->width * race->track->height;
    int map_left = cam.x >> 4;
    int map_right = map_left + 15;
    int map_top = cam.y >> 4;
    int map_bottom = map_top + 10;

    // Handle left and right edges
    for (int y = 0; y < BG_HEIGHT_16; y++)
    {
        int map_y = map_top + y;
        if (cam.x < prev_cam.x)
            draw_tile(map_left, map_y, bg_tiles_offset, race->track);
        else
            draw_tile(map_right, map_y, bg_tiles_offset, race->track);
    }

    // Handle top and bottom edges
    for (int x = 0; x < BG_WIDTH_16; x++)
    {
        int map_x = map_left + x;
        if (cam.y < prev_cam.y)
            draw_tile(map_x, map_top, bg_tiles_offset, race->track);
        else
            draw_tile(map_x, map_bottom, bg_tiles_offset, race->track);
    }

    race->prev_camera = cam;
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------

void draw_tile(int x, int y, int tile_offset, const Track *track)
{
    int track_tile_id;
    int bg_tile_id;

    if (!is_tile_in_map(x, y, track))
    {
        track_tile_id = SE_PALBANK(2);
        bg_tile_id = SE_PALBANK(3);
    } else
    {
        int offset = y * track->width + x;
        track_tile_id = (track->tilemap[offset] << 2) + SE_PALBANK(2);
        bg_tile_id =
                (track->tilemap[tile_offset + offset] << 2) + SE_PALBANK(3);
    }

    x = (x & 15) * 2;
    y = (y & 15) * 32 * 2;

    // Load tiles into the screen block
    se_mem[30][y + x] = track_tile_id;
    se_mem[30][y + x + 1] = track_tile_id + 1;
    se_mem[30][y + 32 + x] = track_tile_id + 2;
    se_mem[30][y + 32 + x + 1] = track_tile_id + 3;
    // Load bg tiles
    se_mem[31][y + x] = bg_tile_id;
    se_mem[31][y + x + 1] = bg_tile_id + 1;
    se_mem[31][y + 32 + x] = bg_tile_id + 2;
    se_mem[31][y + 32 + x + 1] = bg_tile_id + 3;
}

bool is_tile_in_map(int x, int y, const Track *track)
{
    return !(x < 0 || y < 0 || x >= track->width || y >= track->height);
}