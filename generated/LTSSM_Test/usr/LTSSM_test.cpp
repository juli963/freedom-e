#include "LTSSM_test.h"

void LTSSM_TB::write_RX(uint16_t data, uint8_t charisk){
    m_core->io_GTP_data_rx_charisk = charisk;
    m_core->io_GTP_data_rx_data = data;
    tick();
}

void LTSSM_TB::init(){
    reset();
    m_core->io_GTP_data_rx_disperr = 0;
    m_core->io_GTP_data_rx_notintable = 0;
	for (uint i = 0; i<32; i++){
		m_core->reset = 1;
		tick();
	}
	m_core->reset = 0;

    tick();
}