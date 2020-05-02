#include <iostream>
#include "block.h"

block::block() : previous(nullptr), next(nullptr) {
    frame.clear();
}

void block::clear() {
    previous = nullptr;
    next = nullptr;
    frame.clear();
}

void block::print() {
    std::cout << "Previous address: " << previous << std::endl;
    std::cout << "Current address: " << this << std::endl;
    std::cout << "Next address:     " << next << std::endl;
    std::cout << "Frame:";
    frame.print();
}