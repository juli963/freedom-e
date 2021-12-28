#if !defined(I2C_Simulator_H)
    #include "testbench.h"
    #include "stdint.h"
    #include "i2c_to_udp_test.h"
    #include <queue>

    #define I2C_Simulator_H 1
    class I2C_Simulator {
        
        public:
            I2C_Simulator(i2c_to_udp_test_TB *tb) { // Constructor with parameters
                this->tb = tb;
            }
            i2c_to_udp_test_TB *tb;
            uint8_t period = 20;
            bool fail = false;
            bool stop_done = false;
            uint8_t data_count = 0;
            uint8_t counter = 0;
            uint8_t temp = 0;
            enum eState { start, slave_address, rw, address_ack, ack, data, stop };
            eState state = start;

            void init();
            void tick();
           
            struct I2C_Header {   
                uint8_t start:1;
                uint8_t slave_address:7;
                uint8_t rw:1;
                uint16_t length;
                uint8_t stop:1;
            };
            
            std::queue <I2C_Header> fifo_header;
            std::queue <uint8_t> fifo_data;
        private:
            uint8_t reverse(uint8_t b);
    };
#endif
