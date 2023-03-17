
#include "GMII.h"

void GMII_TB::init(){
    m_core->io_intf_rx_RXCLK = 0;
    m_core->io_intf_tx_TXCLK = 0;
    m_core->io_refclock = 0;
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}

void GMII_TB::change_speed(uint8_t speed){
    this->speed = speed;
    m_core->io_stats_Speed = speed;
    if (speed == 0){
            for(uint16_t i = 0; i<800; i++){
                tick();
            }
    }else{
            for(uint16_t i = 0; i<80; i++){
                tick();
            }
    }
}