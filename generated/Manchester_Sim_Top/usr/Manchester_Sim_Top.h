
#include "testbench.h"
#include "../obj_dir/VManchester_Sim_Top.h"
#include "stdint.h"

#if !defined(Manchester_Sim_Top_H)
    #define Manchester_Sim_Top_H 1
    class        Manchester_Sim_Top_TB : public TESTBENCH<VManchester_Sim_Top> {
        public:
            void init();
        private:

    };
#endif
