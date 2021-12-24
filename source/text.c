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

    for (int i = 0; i < 32 * 32; i++)
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

void print_time(SCR_ENTRY *sbb, int x, int y, int frames)
{

    unsigned char time[] = "00:00;00\0";
    int minutes = frames / (60 * 60);
    int seconds = (frames - minutes * 3600) / 60;
    int millis = frames % 60;
    // convert frames (60 frames/s) to a percentage, 60 * 5 / 3 = 100 %
    millis *= 5;
    millis /= 3;
    time[0] += minutes / 10;
    time[1] += minutes % 10;
    time[3] += seconds / 10;
    time[4] += seconds % 10;
    time[6] += millis / 10;
    time[7] += millis % 10;
    print_text(sbb, x, y, (char *) time);
}

void print_number(SCR_ENTRY *sbb, int x, int y, int number)
{
    int orig_num = number;
    unsigned char number_txt[] = "     \0";
    int string_index = 0;
    if (number < 0)
    {
        number *= -1;
        number_txt[0] = 'M';
        string_index++;
    }

    int tenthous = number / 10000;
    number -= tenthous * 10000;
    int thous = number / 1000;
    number -= thous * 1000;
    int hundreds = number / 100;
    number -= hundreds * 100;
    int tens = number / 10;
    int ones = number % 10;

    if (orig_num >= 10000)
    {
        number_txt[string_index] = '0' + tenthous;
        string_index++;
    }
    if (orig_num >= 1000)
    {
        number_txt[string_index] = '0' + thous;
        string_index++;
    }
    if (orig_num >= 100)
    {
        number_txt[string_index] = '0' + hundreds;
        string_index++;
    }
    if (orig_num >= 10)
    {
        number_txt[string_index] = '0' + tens;
        string_index++;
    }
    number_txt[string_index] = '0' + ones;
    print_text(sbb, x, y, (char *) number_txt);
}

void create_textbox(int charblock_base, int screenblock_base, int start_x,
                    int start_y, int w, int h)
{
//    memset32(se_mem[screenblock_base], 0, 32 * 32 / 4);

    // Top/bottom left/right
    se_mem[screenblock_base][start_y * 32 + start_x] = 45 | SE_PALBANK(15);
    se_mem[screenblock_base][start_y * 32 + w + start_x - 1] =
            47 | SE_PALBANK(15);
    se_mem[screenblock_base][(start_y + h - 1) * 32 + start_x] = 51 | SE_PALBANK(15);
    se_mem[screenblock_base][(start_y + h - 1) * 32 + w + start_x - 1] =
            53 | SE_PALBANK(15);
    // Top row
    for (int x = start_x + 1; x < start_x + w - 1; x++)
    {
        se_mem[screenblock_base][start_y * 32 + x] = 46 | SE_PALBANK(15);
    }
    // Bottom row
    for (int x = start_x + 1; x < start_x + w - 1; x++)
    {
        se_mem[screenblock_base][(start_y + h - 1) * 32 + x] =
                52 | SE_PALBANK(15);
    }
    // Middle edges
    for (int y = start_y + 1; y < start_y + h - 1; y++)
    {
        se_mem[screenblock_base][y * 32 + start_x] = 48 | SE_PALBANK(15);
        se_mem[screenblock_base][y * 32 + w + start_x - 1] =
                50 | SE_PALBANK(15);
    }
    // Middle container
    for (int x = start_x + 1; x < start_x + w - 1; x++)
    {
        for (int y = start_y + 1; y < start_y + h - 1; y++)
        {
            se_mem[screenblock_base][y * 32 + x] = 49 | SE_PALBANK(15);
        }
    }
    REG_BLDCNT =
            BLD_STD | BLD_TOP(BLD_BG2) | BLD_BOT(BLD_BG0 | BLD_BG1 | BLD_OBJ);
    REG_BLDALPHA = BLD_EVA(12) | BLD_EVB(10);
    REG_DISPCNT |= DCNT_BG2;
    REG_BG2CNT =
            BG_CBB(charblock_base) | BG_SBB(screenblock_base) | BG_PRIO(1) |
            BG_REG_32x32 | BG_4BPP;
}

void clear_textboxes(int screenblock_base)
{
    memset32(se_mem[screenblock_base], 0, sizeof(SCREENBLOCK));
}
// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------