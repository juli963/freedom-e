#include "../obj_dir/VLTSSM_test.h"
#include "LTSSM_test.h"
#include "PCIe_training.h"



int main(int argc, char **argv) {
	//std::srand(static_cast<int>(time(0)));
	// Calculate Timeouts -> Assume we are using 125MHz Clock
	uint32_t timeout_time_50ms = (uint32_t)(125000000.0*0.050);

	Verilated::commandArgs(argc, argv);
	//TESTBENCH<VLTSSM_test> *tb = new TESTBENCH<VLTSSM_test>();
	LTSSM_TB *tb = new LTSSM_TB();
	PCIe_training *training = new PCIe_training(tb);

	tb->opentrace("trace.vcd");

	struct PCIe_Types::TrainSet s_training;
	s_training.num_link = 5;
	s_training.num_lane = 16;
	s_training.n_fts = 128;
	s_training.data_rate = (1<<1) | (1<<2); // Support 2.5 and 5 GT/s
	s_training.training_control = (1<<2) | (1<<3); // Loopback, Disable Scrambling

	printf("Start\n");
	tb->init();
	training->device_linkinit();
	training->device_linkinit();
	training->device_linkinit();

	for(uint16_t i = 0; i<100; i++){
		training->storage->send_idle(1);
		//training->det_TS[0].Detect(tb->m_core->io_GTP_data_tx_data ,tb->m_core->io_GTP_data_tx_charisk, true);
		//printf("is:%i cons:%i \n", training->det_TS[0].is, training->det_TS[0].is_cons);
	}
	

	printf("Finished \n");
	tb->closetrace();
	/*while(!tb->done()) {
		
	} exit(EXIT_SUCCESS);*/
}

