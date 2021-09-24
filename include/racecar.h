#ifndef TEST_RACECAR_H
#define TEST_RACECAR_H

typedef struct {
    // .12 fixed point
    int x, y;

    // .12 fixed point
    // This is the momentum which gradually gets closer to the proper values
    // to push the car in the direction of "angle".
    int slide_x, slide_y;

    // 0x0000-0xFFFF
    int angle;

    // .12 fixed point
    int speed;

    // .12 fixed point
    int max_speed;
} Racecar;

#endif //TEST_RACECAR_H
