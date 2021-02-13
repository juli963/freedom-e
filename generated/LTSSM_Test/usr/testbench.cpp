#include "../obj_dir/VLTSSM_test.h"
#include "LTSSM_test.h"
#include "PCIe_training.h"
#include "Console_Defines.h"

uint16_t CalculateDllpCRC(uint8_t type, uint8_t* data, uint8_t len = 4);
uint32_t CalculateTlpCRC(uint8_t* data, uint8_t len);

int main(int argc, char **argv) {
	//std::srand(static_cast<int>(time(0)));
	// Calculate Timeouts -> Assume we are using 125MHz Clock
	uint32_t timeout_time_50ms = (uint32_t)(125000000.0*0.050);

	Verilated::commandArgs(argc, argv);
	//TESTBENCH<VLTSSM_test> *tb = new TESTBENCH<VLTSSM_test>();
	LTSSM_TB *tb = new LTSSM_TB();
	LTSSM_TB *tb2 = new LTSSM_TB();
	PCIe_training *training = new PCIe_training(tb);

	tb->opentrace("trace.vcd");
	//tb2->opentrace("trace.vcd");

	struct PCIe_Types::TrainSet s_training;
	s_training.num_link = 5;
	s_training.num_lane = 16;
	s_training.n_fts = 128;
	s_training.data_rate = (1<<1) | (1<<2); // Support 2.5 and 5 GT/s
	s_training.training_control = (1<<2) | (1<<3); // Loopback, Disable Scrambling

	const char *state_names[11] = { "Detect", "Polling", 
                             "Configuration", "Recovery", "L0", "L0s", "L1", "L2", "Disabled", "Loopback", "Hot Reset" };

	enum e_Function{f_device=0, f_root=1};

	printf("Start as Root Port\n");

	tb2->m_core->io_pRootPort = f_device;
	tb->m_core->io_pRootPort = f_root;
	tb->init();
	tb2->init();
	/*training->device_linkinit();
	training->device_linkinit();
	training->device_linkinit();*/

	/*for(uint16_t i = 0; i<100; i++){
		training->storage->send_idle(1);
		//training->det_TS[0].Detect(tb->m_core->io_GTP_data_tx_data ,tb->m_core->io_GTP_data_tx_charisk, true);
		//printf("is:%i cons:%i \n", training->det_TS[0].is, training->det_TS[0].is_cons);
	}*/
	uint32_t state_ff = 0;
	uint32_t substate_ff = 0;
	uint16_t tlp_data[14] = { 	0xFBFD, 0x0000, 0x0074,
								0x0100, 0xC000, 0x5000,
								0x0000, 0x0000, 0x0000, 0x0000,0x0000, 0x0000,
								0x7675, 0x2356
							};
	uint16_t tlp_data2[15] = { 	0xFB00, 0x0000, 0x0074,
								0x0100, 0xC000, 0x5000,
								0x0000, 0x0000, 0x0000, 0x0000,0x0000, 0x0000,
								0x7675, 0x2356,0x00FD
							};
	uint8_t x = 0;
	for(uint32_t i = 0; i<100000; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		if(tb->m_core->io_up > 0){
			CGREEN
			printf("Data Link is Up \n");
			CDEFAULT
			break;
		}else{
			x = 0;
			tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
			tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		}
		

		state_ff = tb->m_core->io_ltssm_state;
		substate_ff = tb->m_core->io_ltssm_substate;
		
		tb->tick();
		tb2->tick();
		if(tb->m_core->io_ltssm_state != state_ff){
			CGREEN
			printf("State has Changed, now: %s \n", state_names[tb->m_core->io_ltssm_state]);
			CDEFAULT
		}
		if(tb->m_core->io_ltssm_substate != substate_ff){
			CGREEN
			printf("Substate has Changed, now: %i \n", tb->m_core->io_ltssm_substate);
			CDEFAULT
		}
	}

	for(uint32_t i = 0; i<500; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb->m_core->io_GTP_data_rx_charisk  = 3;
		tb->m_core->io_GTP_data_rx_data  = 0x5CFD;
		tb->tick();
		tb2->tick();
		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->m_core->io_GTP_data_rx_data  = 0x0060;
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->m_core->io_GTP_data_rx_data  = 0x0000;
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->m_core->io_GTP_data_rx_data  = 0x92d8;
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}

	for(uint32_t i = 0; i<500; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb->m_core->io_GTP_data_rx_charisk  = 2;
		tb->m_core->io_GTP_data_rx_data  = 0x5C00;
		tb->tick();
		tb2->tick();
		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->m_core->io_GTP_data_rx_data  = 0x0060;
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->m_core->io_GTP_data_rx_data  = 0x0000;
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->m_core->io_GTP_data_rx_data  = 0x92d8;
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
		tb->m_core->io_GTP_data_rx_charisk  = 1;
		tb->m_core->io_GTP_data_rx_data  = 0x00FD;
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}

	for(uint32_t i = 0; i<500; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_data  = tlp_data[x];

		if(x == 0){
			tb->m_core->io_GTP_data_rx_charisk  = 3;
		}else{
			tb->m_core->io_GTP_data_rx_charisk  = 0;
			//tb->m_core->io_GTP_data_rx_data  = tlp_data[x]+1;
		}

		x++;
		if(x > 13){
			x = 0;
		}
		tb->tick();
		tb2->tick();
	}
	x = 0;

	for(uint32_t i = 0; i<500; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_data  = tlp_data2[x];

		if(x == 0){
			tb->m_core->io_GTP_data_rx_charisk  = 2;
		}else if(x == 14){
			tb->m_core->io_GTP_data_rx_charisk  = 1;
		}else{
			tb->m_core->io_GTP_data_rx_charisk  = 0;
			//tb->m_core->io_GTP_data_rx_data  = tlp_data2[x]+1;
		}

		x++;
		if(x > 14){
			x = 0;
		}
		tb->tick();
		tb2->tick();
	}
	x = 0;

	// Try TLP Interface
	uint16_t tlp_intf_data[10] = {  0x0074,
									0x0100, 0xC000, 0x5000,
									0x0000, 0x0000, 0x0000, 0x0000,0x0000, 0x0000
								};
	x = 0;
	for(uint32_t i = 0; i<500; i++){
		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;

		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;

		if (x == 0){	// Reset Valid -> Marks end of Package
			tb->m_core->io_tlp_bundle_data_intf_tx_valid = 0;
			tb->m_core->io_tlp_bundle_data_intf_tx_bits_strb = 0;
			tb->m_core->io_tlp_bundle_data_intf_tx_bits_data = 0;


			tb->m_core->io_tlp_bundle_tx_fmt = ((tlp_intf_data[0] & 0x00E0)>>5);
			tb->m_core->io_tlp_bundle_tx_type = ((tlp_intf_data[0] & 0x001F));
			tb->m_core->io_tlp_bundle_tx_tc = ((tlp_intf_data[0] & 0x7000)>>12);
			tb->m_core->io_tlp_bundle_tx_size = sizeof(tlp_intf_data);
			while(tb->m_core->io_tlp_bundle_data_intf_tx_ready > 0){
				tb->tick();
				tb2->tick();

				tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
				tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;

				tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
				tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
			}
			x = 1;

		}else{	// Start Sending Data

			tb->m_core->io_tlp_bundle_data_intf_tx_valid = 1;
			if(x == 10){
				tb->m_core->io_tlp_bundle_data_intf_tx_bits_strb = 1;
			}else{
				tb->m_core->io_tlp_bundle_data_intf_tx_bits_strb = 3;
			}
			
			
			tb->m_core->io_tlp_bundle_data_intf_tx_bits_data = tlp_intf_data[x-1];
			
		}

		

		tb->tick();
		tb2->tick();

		if(tb->m_core->io_tlp_bundle_data_intf_tx_ready > 0){
			x = x+1;
		}
		if(x >= 11){
			x = 0;
		}
	}
	x = 0;

	for(uint32_t i = 0; i<500; i++){
		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;

		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;

		if (x == 0){	// Reset Valid -> Marks end of Package
			tb->m_core->io_tlp_bundle_data_intf_tx_valid = 0;
			tb->m_core->io_tlp_bundle_data_intf_tx_bits_strb = 0;
			tb->m_core->io_tlp_bundle_data_intf_tx_bits_data = 0;


			tb->m_core->io_tlp_bundle_tx_fmt = ((tlp_intf_data[0] & 0x00E0)>>5);
			tb->m_core->io_tlp_bundle_tx_type = ((tlp_intf_data[0] & 0x001F));
			tb->m_core->io_tlp_bundle_tx_tc = ((tlp_intf_data[0] & 0x7000)>>12);
			tb->m_core->io_tlp_bundle_tx_size = sizeof(tlp_intf_data);
			while(tb->m_core->io_tlp_bundle_data_intf_tx_ready > 0){
				tb->tick();
				tb2->tick();

				tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
				tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;

				tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
				tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
			}
			x = 1;

		}else{	// Start Sending Data

			tb->m_core->io_tlp_bundle_data_intf_tx_valid = 1;
			tb->m_core->io_tlp_bundle_data_intf_tx_bits_strb = 3;

			tb->m_core->io_tlp_bundle_data_intf_tx_bits_data = tlp_intf_data[x-1];
			
		}

		

		tb->tick();
		tb2->tick();

		if(tb->m_core->io_tlp_bundle_data_intf_tx_ready > 0){
			x = x+1;
		}
		if(x >= 11){
			x = 0;
		}
	}
	x = 0;

	for(uint32_t i = 0; i<4000; i++){
		if(i > 2000){
			tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
			tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		}
		

		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
	
		if (x == 0){	// Reset Valid -> Marks end of Package
			tb->m_core->io_tlp_bundle_data_intf_tx_valid = 0;
			tb->m_core->io_tlp_bundle_data_intf_tx_bits_strb = 0;
			tb->m_core->io_tlp_bundle_data_intf_tx_bits_data = 0;


			tb->m_core->io_tlp_bundle_tx_fmt = ((tlp_intf_data[0] & 0x00E0)>>5);
			tb->m_core->io_tlp_bundle_tx_type = ((tlp_intf_data[0] & 0x001F));
			tb->m_core->io_tlp_bundle_tx_tc = ((tlp_intf_data[0] & 0x7000)>>12);
			tb->m_core->io_tlp_bundle_tx_size = sizeof(tlp_intf_data);
			while(tb->m_core->io_tlp_bundle_data_intf_tx_ready > 0){
				tb->tick();
				tb2->tick();

				tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
				tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;

				tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
				tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
			}
			x = 1;

		}else{	// Start Sending Data

			tb->m_core->io_tlp_bundle_data_intf_tx_valid = 1;
			if(x == 10){
				tb->m_core->io_tlp_bundle_data_intf_tx_bits_strb = 1;
			}else{
				tb->m_core->io_tlp_bundle_data_intf_tx_bits_strb = 3;
			}
			
			
			tb->m_core->io_tlp_bundle_data_intf_tx_bits_data = tlp_intf_data[x-1];
			
		}

		

		tb->tick();
		tb2->tick();

		if(tb->m_core->io_tlp_bundle_data_intf_tx_ready > 0){
			x = x+1;
		}
		if(x >= 11){
			x = 0;
		}
	}
	x=0;


	// Switch Testbenches -> For receiving ACK Check

	for(uint32_t i = 0; i<500; i++){
		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;

		tb2->m_core->io_GTP_data_rx_data  = tlp_data[x];

		if(x == 0){
			tb2->m_core->io_GTP_data_rx_charisk  = 3;
		}else{
			tb2->m_core->io_GTP_data_rx_charisk  = 0;
			//tb->m_core->io_GTP_data_rx_data  = tlp_data[x]+1;
		}

		x++;
		if(x > 13){
			x = 0;
		}
		tb->tick();
		tb2->tick();
	}
	x = 0;



	for(uint32_t i = 0; i<500; i++){
		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;

		tb2->m_core->io_GTP_data_rx_data  = tlp_data2[x];

		if(x == 0){
			tb2->m_core->io_GTP_data_rx_charisk  = 2;
		}else if(x == 14){
			tb2->m_core->io_GTP_data_rx_charisk  = 1;
		}else{
			tb2->m_core->io_GTP_data_rx_charisk  = 0;
			//tb->m_core->io_GTP_data_rx_data  = tlp_data2[x]+1;
		}

		x++;
		if(x > 14){
			x = 0;
		}
		tb->tick();
		tb2->tick();
	}



	printf("Finished \n");


	printf("Start as Device \n");

	tb2->m_core->io_pRootPort = f_root;
	tb->m_core->io_pRootPort = f_device;
	tb->init();
	tb2->init();

	state_ff = 0;
	substate_ff = 0;
	for(uint32_t i = 0; i<100000; i++){
		if(tb->m_core->io_up > 0){
				tb->m_core->io_GTP_data_rx_charisk  = 3;
				tb->m_core->io_GTP_data_rx_data  = 0x5CFD;
				tb->tick();
				tb->m_core->io_GTP_data_rx_charisk  = 0;
				tb->m_core->io_GTP_data_rx_data  = 0x0060;
				tb->tick();
				tb->m_core->io_GTP_data_rx_charisk  = 0;
				tb->m_core->io_GTP_data_rx_data  = 0x0000;
				tb->tick();
				tb->m_core->io_GTP_data_rx_charisk  = 0;
				tb->m_core->io_GTP_data_rx_data  = 0x92d8;
				tb->tick();
		}else{
			tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
			tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
			tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
			tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;

			
			state_ff = tb->m_core->io_ltssm_state;
			substate_ff = tb->m_core->io_ltssm_substate;
			
			tb->tick();
			tb2->tick();
			if(tb->m_core->io_ltssm_state != state_ff){
				CGREEN
				printf("State has Changed, now: %s \n", state_names[tb->m_core->io_ltssm_state]);
				CDEFAULT
			}
			if(tb->m_core->io_ltssm_substate != substate_ff){
				CGREEN
				printf("Substate has Changed, now: %i \n", tb->m_core->io_ltssm_substate);
				CDEFAULT
			}
		}


	}


	printf("Finished \n");


	tb->closetrace();
	/*while(!tb->done()) {
		
	} exit(EXIT_SUCCESS);*/


	uint8_t tlp_arr[30] = { 	0x00 ,0x00, 0x74, 0x00,
							0x00, 0x01, 0x00, 0xC0, 
							0x00, 0x50,	0x00, 0x00, 
							0x00, 0x00, 0x00, 0x00, 
							0x00, 0x00, 0x00, 0x00, 
							0x00, 0x00,
							0x75, 0x76, 0x56, 0x23,
							0x1C, 0xDF, 0x44, 0x21
						};

	uint8_t arr[5] = {0x00,0x40,0x40,0x19,0xA4};
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x60,arr));
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x50,arr));
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x40,arr));
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x40,arr,6));
	arr[3] = 0xF2; 
	arr[4] = 0xC3; 
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x50,arr,6));

	printf("TLPCRC: 0x%X \n", CalculateTlpCRC(tlp_arr,21));
	printf("TLPCRC: 0x%X \n", CalculateTlpCRC(tlp_arr,22));
}
/* TLP Byte Array
{ 	0xFB, 0x00 ,0x00, 0x74, 0x00,
	0x00, 0x01, 0x00, 0xC0, 0x00, 0x50,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x75, 0x76, 0x56, 0x23,0xFD
}*/
/* TLP Word Array
{ 	0xFBFD, 0x0000, 0x0074,
	0x0100, 0xC000, 0x5000,
	0x0000, 0x0000, 0x0000, 0x0000,0x0000, 0x0000,
	0x7675, 0x2356
}
*/

uint16_t CalculateDllpCRC(uint8_t type, uint8_t* data, uint8_t len)
{
	uint8_t crc_in[len];// = { type, data[0], data[1], data[2] };
	crc_in[0] = type;
	for (uint32_t i = 1; i <= len; i++){
		crc_in[i] = data[i-1];
	}
	uint16_t poly = 0xd008;
	uint16_t crc = 0xffff;

	for(int n=0; n<len; n++)
	{
		uint8_t d = crc_in[n];
		for(int i=0; i<8; i++)
		{
			bool b = ( crc ^ (d >> i) ) & 1;
			crc >>= 1;
			if(b)
				crc ^= poly;
		}
		//printf("CRC%i = 0x%X \n", n, crc);
	}

	return ~( (crc << 8) | ( (crc >> 8) & 0xff) );
}

uint32_t CalculateTlpCRC(uint8_t* data, uint8_t len)
{
	uint32_t poly = 0xedb88320;

	uint32_t crc = 0xffffffff;
	for(size_t n=0; n<len; n++)
	{
		uint8_t d = data[n];
		for(int i=0; i<8; i++)
		{
			bool b = ( crc ^ (d >> i) ) & 1;
			crc >>= 1;
			if(b)
				crc ^= poly;
		}
	}

	return ~(	((crc & 0x000000ff) << 24) |
				((crc & 0x0000ff00) << 8) |
				((crc & 0x00ff0000) >> 8) |
				 (crc >> 24) );
}