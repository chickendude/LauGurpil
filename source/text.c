#include <tonc.h>
#include "text.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

void print_text(SCR_ENTRY *sbb, int x, int y, unsigned char *text)
{
    unsigned char ch;
    unsigned char converted;
    while ((ch = *(text++)) != 0)
    {
        if (ch >= 'A') converted = ch - '0' - 4;
        else if (ch >= '0' && ch <= ';') converted = ch - '0' + 1;
        else if (ch == '.') converted = 39;
        else if (ch == '!') converted = 40;
        else converted = 0;
        sbb[y * 32 + x++] = converted;
    }
}

void print_time(SCR_ENTRY *sbb, int x, int y, int minutes, int seconds, int frames)
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
    print_text(sbb, x, y, time);
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------