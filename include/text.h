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

/**
 * Draws a textbox onto the screen.
 *
 * charblock_base is the location where the text sprite data has been stored.
 * You'll need to have called "prepare_text()" first.
 *
 * screenblock_base is where the tilemap data should be drawn. This should be
 * different from the text data.
 *
 * @param charblock_base Which character block in tile_mem the sprite data has
 * been loaded into.
 * @param screenblock_base Which screenblock to load the tilemap data into.
 * @param x X location of the left edge of the textbox.
 * @param y Y location of the top edge of the textbox.
 * @param w Width of the textbox.
 * @param h Height of the textbox.
 */
void create_textbox(int charblock_base,
               int screenblock_base,
               int x,
               int y,
               int w,
               int h);

void clear_textboxes(int screenblock_base);

#endif //LAUGURPIL_TEXT_H
