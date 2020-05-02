#include <iostream>
#include "input_frame.h"

input_frame::input_frame() {
    clear();
}

void input_frame::clear() {
    frame_header = 0;
    std::fill_n(control, 2, 0);
}

void input_frame::print() {
    std::cout << "Header:   " << std::bitset<8>(frame_header) << std::endl;
    std::cout << "Control:  " << std::bitset<8>(control[0]) << " " << std::bitset<8>(control[1]) << std::endl;
}
