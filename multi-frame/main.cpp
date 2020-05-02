#include "multiple_frame_handler.h"

int main(int argc, char *argv[]) {
    auto d = multiple_frame_handler(15, 7, 0, 0, 1, 2, 4);
    d.disp1();
    d.disp2();
    d.disp3();
    return 0;
}
