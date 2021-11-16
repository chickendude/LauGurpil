#ifndef LAUGURPIL_TEXT_H
#define LAUGURPIL_TEXT_H

#include <tonc.h>

/**
 * Copies the alphanumeric tiles into the specified charblock, the palette into
 * slot 15, and sets up bg3 to display text.
 *
 * @param charblock_base The base charblock to load the tile data into.
 * Genrally this is charblock 3 (0-2 are used for other tilemaps.
 * @param screenblock_base The screenblock to put the text tilemap data (each
 * letter is just a tile id).
 */
void prepare_text(int charblock_base, int screenblock_base);

void print_text(SCR_ENTRY *sbb, int x, int y, const char *text);

void print_time(SCR_ENTRY *sbb, int x, int y, int frames);

void print_speed(SCR_ENTRY *sbb, int x, int y, int speed);

void print_number(SCR_ENTRY *sbb, int x, int y, int number);

#endif //LAUGURPIL_TEXT_H
