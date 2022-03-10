
#include "Console_Defines.h"
#include "Tilelink_ClockCrosser.h"

int main(int argc, char **argv) {

        Tilelink_ClockCrosser_TB *tb = new Tilelink_ClockCrosser_TB();

        tb->opentrace("trace.vcd");
        tb->init();
        tb->do_burst_write();
        tb->init();
        tb->do_burst_read();
}
