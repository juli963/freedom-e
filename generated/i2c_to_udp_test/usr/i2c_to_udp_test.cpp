
#include "i2c_to_udp_test.h"

void i2c_to_udp_test_TB::init(){
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}
