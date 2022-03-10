
#include "testbench.h"
#include "../obj_dir/VTilelink_ClockCrosser.h"
#include "stdint.h"

#if !defined(Tilelink_ClockCrosser_H)
    #define Tilelink_ClockCrosser_H 1
    class        Tilelink_ClockCrosser_TB : public TESTBENCH<VTilelink_ClockCrosser> {
        public:
            void init();
            void do_burst_write();
            void do_burst_read();
        private:

    };
#endif
