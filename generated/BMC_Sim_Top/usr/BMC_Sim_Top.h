
#include "testbench.h"
#include "../obj_dir/VBMC_Sim_Top.h"
#include "stdint.h"

#if !defined(BMC_Sim_Top_H)
    #define BMC_Sim_Top_H 1
    class        BMC_Sim_Top_TB : public TESTBENCH<VBMC_Sim_Top> {
        public:
            void init();
        private:

    };
#endif
