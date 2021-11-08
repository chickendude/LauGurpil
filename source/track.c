#include <tonc.h>
#include "track.h"
#include "constants.h"
#include "race.h"
#include "racecar.h"
// sprite data
#include "ground_tiles.h"
#include "track_tiles.h"
// tilemap data
#include "tracks.h"

const Track *tracks[NUM_TRACKS] = {&track_1, &track_2, &track_3, &track_4};

const Track track_1 = {
        "BASAMORTUA",
        18, 1,
        0x4000,
        track1Width, track1Height,
        16, -30,
        16, 3,
        track1_tilemap
};

const Track track_2 = {
        "LAS TERKA",
        7, 24,
        0x8000,
        track2Width, track2Height,
        -30, 25,
        15, 25,
        track2_tilemap
};

const Track track_3 = {
        "ERREKA",
        28, 12,
        0xC000,
        track3Width, track3Height,
        31, 9,
        31, 17,
        track3_tilemap
};

const Track track_4 = {
        "NON NAGO",
        82, 50,
        0x0000,
        track4Width, track4Height,
        79, 49,
        109, 49,
        track4_tilemap
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
    REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_PRIO(1) | BG_REG_32x32 | BG_4BPP;
    REG_BG1CNT = BG_CBB(1) | BG_SBB(31) | BG_PRIO(2) | BG_REG_32x32 | BG_4BPP;
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

int is_car_in_finish_line(Racecar *car, const Track *track)
{
    int x1 = car->x >> 16;
    int x2 = (car->x + (15 << 12)) >> 16;
    int y1 = car->y >> 16;
    int y2 = (car->y + (15 << 12)) >> 16;

    // Facing left
    if (track->start_angle == 0x4000)
    {
        // in finish line
        if (x1 == track->finish_x1)
        {
            if ((y1 >= track->finish_y1 && y1 <= track->finish_y2) ||
                (y2 >= track->finish_y1 && y2 <= track->finish_y2))
                return 0;
        }
        // past finish line
        if (x1 < track->finish_x1) return 1;
        // behind finish line
        return -1;
    }

        // Facing right
    else if (track->start_angle == 0xC000)
    {
        // in finish line
        if (x1 == track->finish_x1)
        {
            if ((y1 >= track->finish_y1 && y1 <= track->finish_y2) ||
                (y2 >= track->finish_y1 && y2 <= track->finish_y2))
                return 0;
        }
        // past finish line
        if (x1 > track->finish_x1) return 1;
        // behind finish line
        return -1;
    }

        // Facing down
    else if (track->start_angle == 0x8000)
    {
        // in finish line
        if (y1 == track->finish_y1)
        {
            if ((x1 >= track->finish_x1 && x1 <= track->finish_x2) ||
                (x2 >= track->finish_x1 && x2 <= track->finish_x2))
                return 0;
        }
        // past finish line
        if (y1 > track->finish_y1) return 1;
        // behind finish line
        return -1;
    }

        // Facing down
    else
    {
        // in finish line
        if (y1 == track->finish_y1)
        {
            if ((x1 >= track->finish_x1 && x1 <= track->finish_x2) ||
                (x2 >= track->finish_x1 && x2 <= track->finish_x2))
                return 0;
        }
        // past finish line
        if (y1 < track->finish_y1) return 1;
        // behind finish line
        return -1;
    }
}