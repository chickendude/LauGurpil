#include <tonc.h>
#include "text.h"

// -----------------------------------------------------------------------------
// Private function declarations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public function definitions
// -----------------------------------------------------------------------------

void print_text(SCR_ENTRY *sbb, int x, int y, char *text)
{
    unsigned char ch;
    unsigned char converted;
    while((ch = *(text++)) != 0) {
        if (ch > 'A') converted = ch - '0' - 4;
        else if (ch >= '0' && ch <= ';') converted = ch - '0' + 1;
        else if (ch == '.') converted = 39;
        else if (ch == '!') converted = 40;
        else converted = 0;
        sbb[y * 32 + x++] = converted;
    }
}

// -----------------------------------------------------------------------------
// Private functions definitions
// -----------------------------------------------------------------------------