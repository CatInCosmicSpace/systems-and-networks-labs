#ifndef LABS_FRAME_H
#define LABS_FRAME_H

#include <cstdint>

struct frame {
    uint8_t packet_header[3];
    uint8_t frame_header;
    uint8_t data[128];
    uint8_t control[2];

    frame();

    void clear();

    void print();
};


#endif //LABS_FRAME_H
