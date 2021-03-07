 #include "UDP_Appl_test.h"


int main(int argc, char **argv) {
	//std::srand(static_cast<int>(time(0)));

	Verilated::commandArgs(argc, argv);
	UDP_TB *tb = new UDP_TB();



	tb->opentrace("trace.vcd");
	//tb2->opentrace("trace.vcd");

	tb->init();
	uint8_t s = 0;
	tb->m_core->io_ACKBus_tx_ready = 0;
	for(uint32_t i = 0;i<1000000; i++){
		tb->tick();
	}
	tb->m_core->io_ACKBus_tx_ready = 1;
	for(uint32_t i = 0;i<1000000; i++){
		if(tb->m_core->io_ACKBus_tx_valid && s == 0){
			tb->tick();
			tb->m_core->io_ACKBus_tx_ready = 0;
			tb->tick();
			tb->tick();
			tb->m_core->io_ACKBus_tx_ready = 1;
			s = 1;
		}
		if(s == 1 ){
			tb->m_core->io_ACKBus_tx_ready = tb->m_core->io_ACKBus_tx_valid;
		}
		tb->tick();
	}

	tb->closetrace();
	

}
