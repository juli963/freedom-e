
#include "testbench.h"
#include "../obj_dir/Vdram_to_udp_test.h"
#include "stdint.h"

#if !defined(dram_to_udp_test_H)
    #define dram_to_udp_test_H 1
    class        dram_to_udp_test_TB : public TESTBENCH<Vdram_to_udp_test> {
        public:
            void init();
        private:

    };
#endif
