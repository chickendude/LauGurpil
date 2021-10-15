#include <tonc.h>
#include "text.h"
// tile data
#include "lap_numbers.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------
void prepare_text(int charblock_base, int screenblock_base) {
    // Copy number tiles into tile memory and one row of the palette
    memcpy32(tile_mem[charblock_base], lap_numbersTiles + 51 * 8,
             (lap_numbersTilesLen) / 4);
    memcpy32(&pal_bg_mem[15 * 16], lap_numbersPal, 8);

    for (int i = 0; i < 100; i++)
    {
        se_mem[screenblock_base][i] |= SE_PALBANK(15);
    }
    // enable BG0/Mode 3 (bgs 0-3) and load tiles into character block 0 and
    // put the map into screenblock 30
    REG_BG3CNT = BG_CBB(charblock_base) | BG_SBB(screenblock_base) | BG_PRIO(0) | BG_REG_32x32 | BG_4BPP;
}

void print_text(SCR_ENTRY *sbb, int x, int y, char *text)
{
    unsigned char ch;
    unsigned char converted;
    while ((ch = *(text++)) != 0)
    {
        if (ch >= 'A') converted = ch - '0' - 4;
        else if (ch >= '0' && ch <= ';') converted = ch - '0' + 1;
        else if (ch == '.') converted = 39;
        else if (ch == '!') converted = 40;
        else if (ch == ' ') converted = 41;
        else converted = 0;
        sbb[y * 32 + x++] = converted | SE_PALBANK(15);
    }
}

void print_speed(SCR_ENTRY *sbb, int x, int y, int speed)
{
    unsigned char time[] = "    MPH\0";
    speed >>= 7;
    if (speed >= 100)
    {
        time[0] = '1';
        time[1] = '0';
        speed -= 100;
    }

    if (speed >= 10)
    {
        time[1] = '0' + speed / 10;
    }

    time[2] = '0' + speed % 10;
    print_text(sbb, x, y, (char *) time);
}

void
print_time(SCR_ENTRY *sbb, int x, int y, int minutes, int seconds, int frames)
{
    unsigned char time[] = "00:00;00\0";
    // convert millis to a percentage, 60 * 5 / 3 = 100 %
    frames *= 5;
    frames /= 3;
    time[0] += minutes / 10;
    time[1] += minutes % 10;
    time[3] += seconds / 10;
    time[4] += seconds % 10;
    time[6] += frames / 10;
    time[7] += frames % 10;
    print_text(sbb, x, y, (char *) time);
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------