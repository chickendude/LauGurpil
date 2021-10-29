#ifndef LAUGURPIL_CONSTANTS_H
#define LAUGURPIL_CONSTANTS_H

/**
 * The number of 16x16 tiles that will cover the whole width of the map + one
 * extra for scrolling.
 *
 * Our background is 32x32 tiles, but with 16x16 tiles we can only fit 16 in it.
 */
#define BG_WIDTH_16 16

/**
 * The number of 16x16 tiles that will cover the height of the map with one
 * extra so that it shows up when scrolling.
 *
 * 160 / 8 = 20 tiles, but each entry is 16x16 so that's 10 tiles. We draw one
 * extra for scrolling.
 */
#define BG_HEIGHT_16 11

#define MAX_LAPS 10

#define NUM_CARS 7

#define NUM_AI_CARS 4

#define NUM_TOURNAMENTS 2

#define NUM_TRACKS 4

#define SPRITE(n) ((n) * 4)

#endif //LAUGURPIL_CONSTANTS_H
