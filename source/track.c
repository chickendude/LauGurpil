#include <tonc.h>
#include "track.h"
// sprite data
#include "track_tiles.h"
// tilemap data
#include "tracks.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

void load_track()
{
    // CBB = charblock base, where the tile sprites are stored. there are 4 CBB
    // SBB = screenblock base, where the tilemap is stored. there are 32 SBBs
    REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_REG_32x32 | BG_4BPP;
    memcpy32(tile_mem, track_tilesTiles, track_tilesTilesLen / 4);
    memcpy32(&pal_bg_mem[2 * 16], track_tilesPal, 16);

    int x, y;
    int i, j;
    for (i = 0; i < SCREEN_HEIGHT_T / 2; i++)
    {
        for (j = 0; j < SCREEN_WIDTH_T/ 2; j++)
        {
            int tile_id = track1_tilemap[i * track1Width + j] * 4;
            tile_id |= SE_PALBANK(2);
            x = j * 2;
            y = i * 32 * 2;

            int sbb = 30;

            // Load tiles into the screen block
            se_mem[sbb][y + x] = tile_id;
            se_mem[sbb][y + x + 1] = tile_id + 1;
            se_mem[sbb][y + 32 + x] = tile_id + 2;
            se_mem[sbb][y + 32 + x + 1] = tile_id + 3;
        }
    }
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------