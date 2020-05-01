#include "dispatcher.h"

int main(int argc, char* argv[]) {
    auto d = dispatcher(20, 8, 1, 2, 1);
    d.disp1();
    d.disp2();
    //dispatcher(11, 5, 1, 5, 5).check();
    return 0;
}
