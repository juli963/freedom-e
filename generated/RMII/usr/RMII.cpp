
#include "RMII.h"

void RMII_TB::init(){
    m_core->io_intf_REF_CLK = 0x0;
    reset();
    for (uint i = 0; i<32; i++){
            m_core->reset = 1;
            tick();
    }
    m_core->reset = 0;
    tick();
}


/*
class PHYStat() extends Bundle {    
    val Link = Output(Bool())
    val Speed = Output(UInt(4.W))   // 0->10M 1->100M 2-> 1G 3->2.5G 4->5G 5->10G 6->25G 7->40G 8->100G
    val Duplex = Output(Bool())   // True->Full False->Half
}
*/
void RMII_TB::change_speed(uint8_t speed){
    this->speed = speed;
    m_core->io_stats_Speed = speed;

    for(uint16_t i = 0; i<200; i++){
        tick();
    }

}