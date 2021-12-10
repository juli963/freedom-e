
#include "I2C_Sniffer.h"

void I2C_Sniffer_TB::init(){
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}

void I2C_Sniffer_TB::realI2C(){
    if(m_core->io_master_sda_tri > 0){
        m_core->io_main_sda = m_core->io_master_sda_in;
    }else{
        m_core->io_main_sda = m_core->io_master_sda_out;
    }
    
}