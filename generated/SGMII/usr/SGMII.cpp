
#include "SGMII.h"

void SGMII_TB::init(){
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}

void SGMII_TB::set_speed(uint8_t speed){
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

void SGMII_TB::set_link(uint8_t link){
    this->link = link;
    m_core->io_stats_Link = link;
    if (link == 0){
            for(uint16_t i = 0; i<10; i++){
                tick();
            }
    }else{
            for(uint16_t i = 0; i<10; i++){
                tick();
            }
    }
}
