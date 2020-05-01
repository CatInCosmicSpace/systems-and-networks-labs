#include "dispatcher.h"

int main(int argc, char* argv[]) {
    auto d = dispatcher(20, 8, 2, 1, 3, 1, 3);
    d.disp1();
    d.disp2();

    d.disp3();
    //dispatcher(11, 5, 1, 5, 5).check();
    return 0;
}
