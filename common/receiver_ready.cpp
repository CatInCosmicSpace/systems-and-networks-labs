#include <iostream>
#include "receiver_ready.h"

receiver_ready::receiver_ready() {
    clear();
}

void receiver_ready::clear() {
    frame_header = 0;
    std::fill_n(control, 2, 0);
}

void receiver_ready::print() {
    std::cout << "Header:   " << std::bitset<8>(frame_header) << std::endl;
    std::cout << "Control:  " << std::bitset<8>(control[0]) << " " << std::bitset<8>(control[1]) << std::endl;
}
