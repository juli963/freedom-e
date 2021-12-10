#if !defined(I2C_Slave_H)
    #include "testbench.h"
    #include "stdint.h"
    #include "I2C_Sniffer.h"
    #include <queue>

    #define I2C_Slave_H 1
    class I2C_Slave {
        
        public:
            I2C_Slave(I2C_Sniffer_TB *tb) { // Constructor with parameters
                this->tb = tb;
            }
            I2C_Sniffer_TB *tb;
            void init();
            void tick();
           
        private:

    };
#endif
