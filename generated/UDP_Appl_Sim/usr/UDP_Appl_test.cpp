#include "UDP_Appl_test.h"

void UDP_TB::init(){
    reset();
    m_core->io_connected = 0;


	for (uint i = 0; i<32; i++){
		m_core->reset = 1;
		tick();
	}
	m_core->reset = 0;
    m_core->io_connected = 1;

    tick();
}