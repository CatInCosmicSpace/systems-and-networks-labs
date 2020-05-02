#ifndef LABS_BLOCK_H
#define LABS_BLOCK_H

#include <bitset>
#include <vector>
#include <array>
#include "frame.h"

struct block {
    block *_previous;
    block *_next;
    frame _frame;

    block();

    void clear();

    void print();
};

#endif //LABS_BLOCK_H
