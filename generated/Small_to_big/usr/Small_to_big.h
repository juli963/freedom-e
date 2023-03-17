
#include "testbench.h"
#include "../obj_dir/VSmall_to_big.h"
#include "stdint.h"

#if !defined(Small_to_big_H)
    #define Small_to_big_H 1
    class        Small_to_big_TB : public TESTBENCH<VSmall_to_big> {
        public:
            void init();
        private:

    };
#endif
