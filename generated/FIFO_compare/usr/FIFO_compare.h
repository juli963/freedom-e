
#include "testbench.h"
#include "../obj_dir/VFIFO_compare.h"
#include "stdint.h"

#if !defined(FIFO_compare_H)
    #define FIFO_compare_H 1
    class        FIFO_compare_TB : public TESTBENCH<VFIFO_compare> {
        public:
            void init();
        private:

    };
#endif
