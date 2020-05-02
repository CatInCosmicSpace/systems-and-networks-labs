#include <iostream>
#include "queue_characteristics.h"

queue_characteristics::queue_characteristics()
        : first(nullptr), last(nullptr), length(0) {}

queue_characteristics::queue_characteristics(block *_first, block *_last, size_t _length)
        : first(_first), last(_last), length(_length) {}

void queue_characteristics::clear() {
    first = nullptr;
    last = nullptr;
    length = 0;
}

void queue_characteristics::print() {
    std::cout << "First element address: " << first << std::endl;
    std::cout << "Last element address:  " << last << std::endl;
    std::cout << "Queue length:          " << length << std::endl;
}
