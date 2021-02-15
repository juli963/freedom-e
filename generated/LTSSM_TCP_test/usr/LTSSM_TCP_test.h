#include "testbench.h"
#include "../obj_dir/VLTSSM_TCP_test.h"
#include "stdint.h"

#if !defined(LTSSM_H)
    #define LTSSM_H 1
    class	LTSSM_TB : public TESTBENCH<VLTSSM_TCP_test> {
        public:
            void write_RX(uint16_t data, uint8_t charisk);
            void init();
            
        private:

    };
#endif