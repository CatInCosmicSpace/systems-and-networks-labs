#ifndef LABS12_RECEIVER_READY_H
#define LABS12_RECEIVER_READY_H

#include <bitset>
#include <vector>
#include <array>

struct receiver_ready {
    uint8_t frame_header;
    uint16_t control;
};

#endif //LABS_RECEIVER_READY_H
