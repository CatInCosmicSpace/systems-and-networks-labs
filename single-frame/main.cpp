#include "single_frame_handler.h"

int main(int argc, char *argv[]) {
    auto d = single_frame_handler(15, 7, 0, 0, 2);
    d.disp1();
    d.disp2();
    //single_frame_handler(11, 5, 1, 5, 5).check();
    return 0;
}
