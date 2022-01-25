
#include "testbench.h"
#include "../obj_dir/Vc8b10b_to_udp_test.h"
#include "stdint.h"

#if !defined(c8b10b_to_udp_test_H)
    #define c8b10b_to_udp_test_H 1
    class        c8b10b_to_udp_test_TB : public TESTBENCH<Vc8b10b_to_udp_test> {
        public:
            void init();
        private:

    };
#endif
