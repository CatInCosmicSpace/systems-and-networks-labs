#include "queue_characteristics.h"
#include "easylogging++.h"

queue_characteristics::queue_characteristics()
    :first(nullptr), last(nullptr), length(0) {}

queue_characteristics::queue_characteristics(block* _first, block* _last, size_t _length)
: first(_first), last(_last), length(_length) {}

void queue_characteristics::clear() {
    first = nullptr;
    last = nullptr;
    length = 0;
}

void queue_characteristics::print() {
    LOG(INFO) << "First element address: " << first;
    LOG(INFO) << "Last element address:  " << last;
    LOG(INFO) << "Queue length:          " << length;
}
