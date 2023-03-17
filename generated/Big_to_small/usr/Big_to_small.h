
#include "testbench.h"
#include "../obj_dir/VBig_to_small.h"
#include "stdint.h"

#if !defined(Big_to_small_H)
    #define Big_to_small_H 1
    class        Big_to_small_TB : public TESTBENCH<VBig_to_small> {
        public:
            void init();
        private:

    };
#endif
