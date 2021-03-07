#include "testbench.h"
#include "../obj_dir/VUDP_Appl_Sim.h"
#include "stdint.h"

#if !defined(UDP_H)
    #define UDP_H 1
    class	UDP_TB : public TESTBENCH<VUDP_Appl_Sim> {
        public:
            
            void init();
            
        private:

    };
#endif