#include "block.h"
#include "easylogging++.h"

block::block(): previous(nullptr), next(nullptr) {
    frame.clear();
}

void block::clear() {
    previous = nullptr;
    next = nullptr;
    frame.clear();
}

void block::print() {
    LOG(INFO) << "Previous address: " << previous;
    LOG(INFO) << "Current address: " << this;
    LOG(INFO) << "Next address:     " << next;
    LOG(INFO) << "Frame:";
    frame.print();
}