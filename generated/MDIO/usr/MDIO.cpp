#include "MDIO.h"

void MDIO_TB::init(){
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}

void MDIO_TB::write(uint8_t phy, uint8_t reg, uint16_t data){
    m_core->io_signals_phyad = phy;
    m_core->io_signals_regad = reg;
    m_core->io_signals_data_wr = data;
    m_core->io_signals_opcode = 1;
    set_start();
}

void MDIO_TB::read(uint8_t phy, uint8_t reg){
    m_core->io_signals_phyad = phy;
    m_core->io_signals_regad = reg;
    m_core->io_signals_opcode = 2;
    set_start();
}

uint16_t MDIO_TB::read_data(){
    return m_core->io_signals_data_rd;
}

bool MDIO_TB::is_busy(){
    return m_core->io_signals_busy != 0;
}
void MDIO_TB::set_start(){
    m_core->io_signals_start = 1;
}
void MDIO_TB::reset_start(){
    m_core->io_signals_start = 0;
}            