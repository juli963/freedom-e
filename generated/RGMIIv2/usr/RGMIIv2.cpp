
#include "RGMIIv2.h"

void RGMIIv2_TB::init(){
    m_core->io_intf_rx_RXC = 0;
    tx_temp = m_core->io_intf_tx_TXC;
    m_core->io_crs_in = 0;
    //tx_clock = 0;
    m_core->io_refclock = 0;
    m_core->io_stats_Speed = 0;
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}

void RGMIIv2_TB::change_speed(uint8_t speed){
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