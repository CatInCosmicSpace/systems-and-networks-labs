#include <algorithm>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "frame.h"

frame::frame() {
    clear();
}

void frame::clear() {
    std::fill_n(packet_header, 3, 0);
    frame_header = 0;
    std::fill_n(data, 128, 0);
    std::fill_n(control, 2, 0);
}

void frame::print() {
    std::cout << "Frame information:    " << std::endl;
    std::cout << "Packet header:        "
              << std::bitset<8>(packet_header[0]) << " "
              << std::bitset<8>(packet_header[1]) << " "
              << std::bitset<8>(packet_header[2]) << std::endl;
    std::cout << "Frame header:         " << std::bitset<8>(frame_header) << std::endl;
    std::stringstream data_string;
    for (auto i : data)
        data_string << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << int(i) << " ";
    std::cout << "Data:                 " << std::endl << data_string.str() << std::endl;
    std::cout << "Control:              "
              << std::bitset<8>(control[0]) << " "
              << std::bitset<8>(control[1]) << std::endl;
}