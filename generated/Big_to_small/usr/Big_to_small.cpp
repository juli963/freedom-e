
#include "Big_to_small.h"

void Big_to_small_TB::init(){
    reset();

        for (uint8_t i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}
