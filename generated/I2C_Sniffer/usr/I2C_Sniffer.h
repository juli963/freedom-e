
#include "testbench.h"
#include "../obj_dir/VI2C_Sniffer.h"
#include "stdint.h"

#if !defined(I2C_Sniffer_H)
    #define I2C_Sniffer_H 1
    class        I2C_Sniffer_TB : public TESTBENCH<VI2C_Sniffer> {
        public:
            void init();
            void realI2C();
        private:

    };
#endif
