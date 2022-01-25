
#include "c8b10b_to_udp_test.h"

void c8b10b_to_udp_test_TB::init(){
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}
