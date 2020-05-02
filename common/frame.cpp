#include <algorithm>
#include <bitset>
#include <iomanip>
#include "frame.h"
#include "easylogging++.h"

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
    LOG(INFO) << "Frame information:    ";
    LOG(INFO) << "Packet header:        "
                << std::bitset<8>(packet_header[0]) << " "
                << std::bitset<8>(packet_header[1]) << " "
                << std::bitset<8>(packet_header[2]);
    LOG(INFO) << "Frame header:         " << std::bitset<8>(frame_header);
    std::stringstream data_string;
    for (auto i : data)
        data_string << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << int(i) << " ";
    LOG(INFO) << "Data:                 " << std::endl << data_string.str();
    LOG(INFO) << "Control:              "
                << std::bitset<8>(control[0]) << " "
                << std::bitset<8>(control[1]);
}