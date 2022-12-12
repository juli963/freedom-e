
#include "testbench.h"
#include "../obj_dir/VETHClock.h"
#include "stdint.h"

#if !defined(ETHClock_H)
    #define ETHClock_H 1
    class        ETHClock_TB : public TESTBENCH<VETHClock> {
        public:
            void init();
            enum ETH_Speeds { E_10MB=0, E_100MB, E_1G };
        private:
            
    };
#endif
