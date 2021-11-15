
#include "testbench.h"
#include "../obj_dir/VNRZI_Sim_Top.h"
#include "stdint.h"

#if !defined(NRZI_Sim_Top_H)
    #define NRZI_Sim_Top_H 1
    class        NRZI_Sim_Top_TB : public TESTBENCH<VNRZI_Sim_Top> {
        public:
            void init();
        private:

    };
#endif
