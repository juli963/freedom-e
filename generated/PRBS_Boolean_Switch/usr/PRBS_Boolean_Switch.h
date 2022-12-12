
#include "testbench.h"
#include "../obj_dir/VPRBS_Boolean_Switch.h"
#include "stdint.h"

#if !defined(PRBS_Boolean_Switch_H)
    #define PRBS_Boolean_Switch_H 1
    class        PRBS_Boolean_Switch_TB : public TESTBENCH<VPRBS_Boolean_Switch> {
        public:
            void init();

            void init_polynom(uint8_t* arr, uint8_t len, uint8_t width);
            uint64_t get_next_lfsr(uint64_t input);
            uint8_t calculate_bool(uint64_t seed);

        private:
			uint64_t PRBS_polynom = 0;
			uint8_t lfsr_width = 0;
            uint64_t lfsr_mask = 0;

    };
#endif
