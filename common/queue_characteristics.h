#ifndef LABS_QUEUE_CHARACTERISTICS_H
#define LABS_QUEUE_CHARACTERISTICS_H

#include "block.h"

struct queue_characteristics {
    block* first;
    block* last;
    size_t length;

    queue_characteristics();
    queue_characteristics(block*, block*, size_t);

    void clear();
    void print();
};


#endif //LABS_QUEUE_CHARACTERISTICS_H
