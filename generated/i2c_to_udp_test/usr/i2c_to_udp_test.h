
#include "testbench.h"
#include "../obj_dir/Vi2c_to_udp_test.h"
#include "stdint.h"

#if !defined(i2c_to_udp_test_H)
    #define i2c_to_udp_test_H 1
    class        i2c_to_udp_test_TB : public TESTBENCH<Vi2c_to_udp_test> {
        public:
            void init();
        private:

    };
#endif
