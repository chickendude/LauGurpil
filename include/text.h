#ifndef LAUGURPIL_TEXT_H
#define LAUGURPIL_TEXT_H

#include <tonc.h>

void print_text(SCR_ENTRY *sbb, int x, int y, unsigned char *text);

void
print_time(SCR_ENTRY *sbb, int x, int y, int minutes, int seconds, int frames);

#endif //LAUGURPIL_TEXT_H
