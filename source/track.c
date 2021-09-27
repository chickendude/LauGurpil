#include <tonc.h>
#include "track.h"
#include "constants.h"
#include "race.h"
// sprite data
#include "track_tiles.h"
// tilemap data
#include "tracks.h"

const Track track_1 = {
        18, 1,
        0x4000,
        track1Width, track1Height,
        track1_tilemap
};
// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------

void draw_tile(int x, int y, int tile_id);

bool is_tile_in_map(int x, int y, const Track *track);

// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

void load_track(const Track *track, Camera *camera)
{
    // CBB = charblock base, where the tile sprites are stored. there are 4 CBB
    // SBB = screenblock base, where the tilemap is stored. there are 32 SBBs
    REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_REG_32x32 | BG_4BPP;
    memcpy32(tile_mem, track_tilesTiles, track_tilesTilesLen / 4);
    memcpy32(&pal_bg_mem[2 * 16], track_tilesPal, 16);

    camera->x = (track->start_x << 4) - SCREEN_WIDTH / 2;
    camera->y = (track->start_y << 4) - SCREEN_HEIGHT / 2;

    int x, y;
    int start_x = camera->x >> 4;
    int start_y = camera->y >> 4;
    for (y = start_y; y < BG_HEIGHT_16 + start_y; y++)
    {
        for (x = start_x; x < BG_WIDTH_16 + start_x; x++)
        {
            int tile_id = is_tile_in_map(x, y, track) ?
                          track->tilemap[y * track->width + x] * 4 : 0;
            draw_tile(x, y, tile_id);
        }
    }
}

void update_tilemap(Race *race)
{
    Camera cam = race->camera;
    int map_w = race->track->width;
    const unsigned char *tilemap = race->track->tilemap;
    int map_left = cam.x >> 4;
    int map_right = map_left + 15;
    int map_top = cam.y >> 4;
    int map_bottom = map_top + 10;

    // Handle left and right edges
    for (int y = 0; y < BG_HEIGHT_16; y++)
    {
        int map_y = map_top + y;
        int tile_id = is_tile_in_map(map_left, map_y, race->track) ?
                      tilemap[map_y * map_w + map_left] : 0;
        draw_tile(map_left, map_y, tile_id * 4);

        tile_id = is_tile_in_map(map_right, map_y, race->track) ?
                  tilemap[map_y * map_w + map_right] : 0;
        draw_tile(map_right, map_y, tile_id * 4);
    }

    // Handle top and bottom edges
    for (int x = 0; x < BG_WIDTH_16; x++)
    {
        int map_x = map_left + x;
        int tile_id = is_tile_in_map(map_x, map_top, race->track) ?
                      tilemap[map_top * map_w + map_x] : 0;
        draw_tile(map_x, map_top, tile_id * 4);

        tile_id = is_tile_in_map(map_x, map_bottom, race->track) ?
                  tilemap[map_bottom * map_w + map_x] : 0;
        draw_tile(map_x, map_bottom, tile_id * 4);
    }

}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------

void draw_tile(int x, int y, int tile_id)
{
    x = (x & 15) * 2;
    y = (y & 15) * 32 * 2;
    tile_id |= SE_PALBANK(2);

    // Load tiles into the screen block
    se_mem[30][y + x] = tile_id;
    se_mem[30][y + x + 1] = tile_id + 1;
    se_mem[30][y + 32 + x] = tile_id + 2;
    se_mem[30][y + 32 + x + 1] = tile_id + 3;
}

bool is_tile_in_map(int x, int y, const Track *track)
{
    return !(x < 0 || y < 0 || x >= track->width || y >= track->height);
}