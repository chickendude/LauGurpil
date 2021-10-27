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
void prepare_text(int charblock_base, int screenblock_base)
{
    // Copy number tiles into tile memory and one row of the palette
    memcpy32(tile_mem[charblock_base], lap_numbersTiles + 51 * 8,
             (lap_numbersTilesLen) / 4);
    memcpy32(&pal_bg_mem[15 * 16], lap_numbersPal, 8);

    for (int i = 0; i < 32*32; i++)
    {
        se_mem[screenblock_base][i] = SE_PALBANK(15);
    }
    // enable BG0/Mode 3 (bgs 0-3) and load tiles into supplied character block
    // and the map into supplied screenblock
    REG_BG3CNT =
            BG_CBB(charblock_base) | BG_SBB(screenblock_base) | BG_PRIO(0) |
            BG_REG_32x32 | BG_4BPP;
}

void print_text(SCR_ENTRY *sbb, int x, int y, const char *text)
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
        else if (ch == '<') converted = 42;
        else if (ch == '=') converted = 43;
        else if (ch == '>') converted = 44;
        else converted = 0;
        sbb[y * 32 + x++] = converted | SE_PALBANK(15);
    }
}

void print_speed(SCR_ENTRY *sbb, int x, int y, int speed)
{
    unsigned char time[] = "    MPH\0";
    if (speed < 0)
    {
        speed *= -1;
        if (speed >> 7 >= 10) time[0] = 'M';
        else time[1] = 'M';
    }

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

void print_number(SCR_ENTRY *sbb, int x, int y, int number)
{
    unsigned char number_txt[] = "     \0";
    int tenthous = number / 10000;
    number -= tenthous * 10000;
    int thous = number / 1000;
    number -= thous * 1000;
    int hundreds = number / 100;
    number -= hundreds * 100;
    int tens = number / 10;
    int ones = number % 10;

    int i = 0;
    if (tenthous > 0)
    {
        number_txt[i] = '0' + tenthous;
        i++;
    }
    if (thous > 0)
    {
        number_txt[i] = '0' + thous;
        i++;
    }
    if (hundreds > 0)
    {
        number_txt[i] = '0' + hundreds;
        i++;
    }
    if (tens > 0)
    {
        number_txt[i] = '0' + tens;
        i++;
    }
    number_txt[i] = '0' + ones;
    print_text(sbb, x, y, (char *) number_txt);
}
// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------