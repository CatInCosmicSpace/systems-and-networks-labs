#ifndef LABS_RECEIVER_READY_H
#define LABS_RECEIVER_READY_H

#include <bitset>
#include <vector>
#include <array>

struct receiver_ready {
    uint8_t frame_header;
    uint8_t control[2];

    receiver_ready();

    void clear();
    void print();
};

#endif //LABS_RECEIVER_READY_H
