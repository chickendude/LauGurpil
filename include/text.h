#ifndef LAUGURPIL_TEXT_H
#define LAUGURPIL_TEXT_H

#include <tonc.h>

void prepare_text(int charblock_base, int screenblock_base);

void print_text(SCR_ENTRY *sbb, int x, int y, char *text);

void
print_time(SCR_ENTRY *sbb, int x, int y, int minutes, int seconds, int frames);

void print_speed(SCR_ENTRY *sbb, int x, int y, int speed);

void print_number(SCR_ENTRY *sbb, int x, int y, int number);

#endif //LAUGURPIL_TEXT_H
