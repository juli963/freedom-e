
#include "Small_to_big.h"

void Small_to_big_TB::init(){
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}
