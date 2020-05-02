#include "dispatcher.h"

int main(int argc, char *argv[]) {
    auto d = dispatcher(15, 7, 0, 0, 2);
    d.disp1();
    d.disp2();
    //dispatcher(11, 5, 1, 5, 5).check();
    return 0;
}
