
#include "InbandDecoder.h"

void InbandDecoder_TB::init(){
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}

void InbandDecoder_TB::setStats(uint8_t speed, bool link, bool duplex){
    m_core->io_RX_Data_Rising_0 = 0;
    m_core->io_RX_Data_Rising_1 = 0;
    m_core->io_RX_Data_Rising_2 = 0;
    m_core->io_RX_Data_Rising_3 = 0;

    if((speed & 0x01) > 0){
        m_core->io_RX_Data_Rising_1 = 1;
    }
    if((speed & 0x02) > 0){
        m_core->io_RX_Data_Rising_2 = 1;
    }

    if(link){
        m_core->io_RX_Data_Rising_0 = 1;
    }
    if(duplex){
        m_core->io_RX_Data_Rising_3 = 1;
    }
}

void InbandDecoder_TB::setValid(){
    m_core->io_RX_CTL_Rising = 1;   
}

void InbandDecoder_TB::resetValid(){
    m_core->io_RX_CTL_Rising = 0;   
}