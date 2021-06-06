
#include "testbench.h"
#include "../obj_dir/VCfg_Mem.h"
#include "stdint.h"

#if !defined(Cfg_Mem_H)
    #define Cfg_Mem_H 1
    class        Cfg_Mem_TB : public TESTBENCH<VCfg_Mem> {
        public:
            void init();
        private:

    };
#endif
