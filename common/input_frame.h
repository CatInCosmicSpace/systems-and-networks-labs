#ifndef LABS_INPUT_FRAME_H
#define LABS_INPUT_FRAME_H

#include <bitset>
#include <vector>
#include <array>

struct input_frame {
    uint8_t frame_header;
    uint8_t control[2];

    input_frame();

    void clear();

    void print();
};

#endif //LABS_INPUT_FRAME_H
