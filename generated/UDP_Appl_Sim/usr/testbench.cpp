 #include "UDP_Appl_test.h"


int main(int argc, char **argv) {
	//std::srand(static_cast<int>(time(0)));

	Verilated::commandArgs(argc, argv);
	UDP_TB *tb = new UDP_TB();



	tb->opentrace("trace.vcd");
	//tb2->opentrace("trace.vcd");

/*
        val connected = Input(Bool())
        val ackseq = Input(Vec(4,UInt(16.W)))
        val nextseq = Input(Vec(4,UInt(16.W)))
        val retransmit = (DecoupledIO(Output(new UDPDataBus(8))))
        val master = (DecoupledIO(Output(new UDPDataBus(8))))
*/
	tb->m_core->io_ackseq_0 = 0;
	tb->m_core->io_ackseq_1 = 0;
	tb->m_core->io_ackseq_2 = 0;
	tb->m_core->io_ackseq_3 = 0;

	tb->m_core->io_nextseq_0 = 0;
	tb->m_core->io_nextseq_1 = 0;
	tb->m_core->io_nextseq_2 = 0;
	tb->m_core->io_nextseq_3 = 0;

	tb->init();
	uint8_t s = 0;

	for(uint32_t i = 0;i<1000000; i++){
		tb->tick();
		if(tb->m_core->io_master_valid){
			tb->m_core->io_nextseq_0 = tb->m_core->io_nextseq_0 +1;
		}
		tb->m_core->io_master_ready = tb->m_core->io_master_valid;
		tb->m_core->io_retransmit_ready = tb->m_core->io_retransmit_valid;

	}
	
	for(uint32_t i = 0;i<1000000; i++){

		tb->tick();
	}

	tb->closetrace();
	

}
