#include <iostream>
#include "block.h"

block::block() : _previous(nullptr), _next(nullptr) {
    _frame.clear();
}

void block::clear() {
    _previous = nullptr;
    _next = nullptr;
    _frame.clear();
}

void block::print() {
    std::cout << "Previous address: " << _previous << std::endl;
    std::cout << "Current address: " << this << std::endl;
    std::cout << "Next address:     " << _next << std::endl;
    std::cout << "Frame:";
    _frame.print();
}