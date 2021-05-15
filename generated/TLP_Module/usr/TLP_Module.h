#include "testbench.h"
#include "../obj_dir/VTLP_Module.h"
#include "stdint.h"
#include <queue>

#if !defined(TLP_Module_H)
    #define TLP_Module_H 1
    class        TLP_Module_TB : public TESTBENCH<VTLP_Module> {
        public:
            uint8_t id;
            std::queue<uint16_t> qdata;
            std::queue<uint16_t> qstrb;
            TLP_Module_TB(uint8_t x) { 
                id = x;
            }
            void init();
            void queue(TLP_Module_TB* otb);
        private:

    };
#endif
