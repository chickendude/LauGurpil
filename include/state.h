#ifndef TEST_STATE_H
#define TEST_STATE_H

struct State {
    void (*initialize)();
    void (*update)();
    void (*input)();
};
#endif //TEST_STATE_H
