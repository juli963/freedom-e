//#include "../obj_dir/VLTSSM_TCP_test.h"
#include "TCP_Bus.h"
#include "Console_Defines.h"
 #include "LTSSM_TCP_test.h"

uint16_t CalculateDllpCRC(uint8_t type, uint8_t* data, uint8_t len = 4);
uint32_t CalculateTlpCRC(uint8_t* data, uint8_t len);
void decode_TLP(uint8_t* data, uint8_t len);
void decode_DLLP(uint8_t* data, uint8_t len);
uint8_t tlp_data_format(uint8_t format, uint8_t type);
void decode_Stream();

void wait_Clock(LTSSM_TB *tb, LTSSM_TB *tb2, uint32_t n);
void send_initTLP(LTSSM_TB *tb, LTSSM_TB *tb2, TCP_Bus *tcp_stack );
void send_initTLP_back(LTSSM_TB *tb, LTSSM_TB *tb2, TCP_Bus *tcp_stack );
int scramble_byte(int inbyte);

bool TrainingSequence = false;
uint16_t lfsr_g = 0;
int main(int argc, char **argv) {
	//std::srand(static_cast<int>(time(0)));
	// Calculate Timeouts -> Assume we are using 125MHz Clock
	uint32_t timeout_time_50ms = (uint32_t)(125000000.0*0.050);

	Verilated::commandArgs(argc, argv);
	//TESTBENCH<VLTSSM_test> *tb = new TESTBENCH<VLTSSM_test>();
	LTSSM_TB *tb = new LTSSM_TB();
	LTSSM_TB *tb2 = new LTSSM_TB();

	TCP_Bus *tcp_stack = new TCP_Bus(tb,tb2);

	tcp_stack->Prepare_Address(0, 0xC0A802D4, 0x111213141516);
	tcp_stack->Prepare_Address(1, 0xC0A802D5, 0x111213141517);


	tb->opentrace("trace.vcd");
	//tb2->opentrace("trace.vcd");

	const char *state_names[11] = { "Detect", "Polling", 
                             "Configuration", "Recovery", "L0", "L0s", "L1", "L2", "Disabled", "Loopback", "Hot Reset" };

	enum e_Function{f_device=0, f_root=1};

	printf("Start as Root Port\n");

	tb2->m_core->io_pRootPort = f_device;
	tb->m_core->io_pRootPort = f_root;
	tb->init();

	for(uint32_t i = 0; i<20; i++){
		tb->m_core->io_GTP_data_rx_data  = 0;
		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->tick();

	}


	uint16_t ts_data[] = {
		0x1BC,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
		0x1BC,0x01,0x02,0x03,0x04,0x05,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x1BC,
		0x1BC,0x10,0x11,0x12,0x13,0x14,0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,0x4A,
		0x0BC,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x05,0x00,0x00,0x00,0x00,0x00,0x00,
		0x0BC,0x01,0x11C,0x1BC,0x11C,0x11C,0x06,0x1BC,0x1FB,0x05,0x1FB,0x00,0x00,0x00,0x00,
		0x1BC,0x11C,0x11C,0x11C,0xFF,0x17,0xC0,0x14,
		0x1BC,0xFF,0x17,0x40,0x14,0xb2,0xe7,0x02,0x82,0x72,0x6e,0x28,0xa6,0xbe,0x6d,0xbf,0x8d,0xbe,0x40,0xa7,0xe6,0x2c,0xd3,0xe2,0xb2,0x07,0x15c,0x37,0x33,0xc2,0xcb,0xdc,0x51,0x1fd,0x15c,0x64,0xa8,0x94,0x5e,0xdf,0x93,0x1fd
	};

	tb->m_core->io_sim_state = 10; // Scrambler Test
	tb->m_core->io_scramble_enable = 0x1;
	tb2->m_core->io_scramble_enable = 0x1;

	uint32_t end_train = 0;
	for(uint32_t i = 0; i<sizeof(ts_data)/2; i=i+2){
		uint8_t scrambled[2];
		
		if(i > end_train && TrainingSequence){
			TrainingSequence = false;
			printf("Training End: %i \n", i);
			printf("Training End: %i \n", i);
			printf("Char: 0x%X \n", ts_data[i+1]);
			printf("LFSR: 0x%X \n", lfsr_g);
		}
		if(ts_data[i] == 0x1BC && (ts_data[i+15] == 0x45 || ts_data[i+15] == 0x4A) ){
			TrainingSequence = true;
			end_train = i+15;
			printf("End_Train Set: %i, %i \n", i,end_train);
		}
		scrambled[0] = scramble_byte(ts_data[i]) & 0xFF;

		if( (i+1) > end_train && TrainingSequence){
			TrainingSequence = false;
			printf("Training End: %i \n", i);
			printf("Char: 0x%X \n", ts_data[i+1]);
			printf("LFSR: 0x%X \n", lfsr_g);
		}

		if(ts_data[i+1] == 0x1BC && (ts_data[i+1+15] == 0x45 || ts_data[i+1+15] == 0x4A) ){
			TrainingSequence = true;
			end_train = i+1+15;
			printf("End_Train Set: %i, %i \n", i,end_train);
		}
		scrambled[1] = scramble_byte(ts_data[i+1]) & 0xFF;

		tb->m_core->io_GTP_data_rx_data  = scrambled[0] | ((uint16_t)scrambled[1]<<8);
		tb->m_core->io_GTP_data_rx_charisk  = ((ts_data[i]&0x100)>>8) | ((ts_data[i+1]&0x100)>>7);
		tb->tick();

	}
	

	for(uint32_t i = 0; i<20; i++){
		tb->m_core->io_GTP_data_rx_data  = 0;
		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->tick();

	}
	tb->m_core->io_sim_state = 0; // Scrambler Test
	tb->init();
	tb2->init();



	//tb->tick();
	//tb->tick();
	//tb->tick();
	tcp_stack->Connect(1,0xC0A802D4,0x111213141516, 15000);
	tcp_stack->Connect(0,0xC0A802D5,0x111213141517, 15000);
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
	/*uint16_t tlp_data[14] = { 	0xFBFD, 0x0000, 0x0074,
								0x0100, 0xC000, 0x5000,
								0x0000, 0x0000, 0x0000, 0x0000,0x0000, 0x0000,
								0x7675, 0x2356
							};
	uint16_t tlp_data2[15] = { 	0xFB00, 0x0000, 0x0074,
								0x0100, 0xC000, 0x5000,
								0x0000, 0x0000, 0x0000, 0x0000,0x0000, 0x0000,
								0x7675, 0x2356,0x00FD
							};*/

	

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
	tb->m_core->io_sim_state = 1; // Link Up

	uint8_t tlp_data[]  = { 0xFB, 0x00 ,0x00, 0x74, 0x00,
							0x00, 0x01, 0x00, 0xC0, 0x00, 0x50,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x75, 0x76, 0x56, 0x23,0xFD
	};
	uint8_t tlp_dat[]  = {  0x00 ,0x00, 0x74, 0x00,
							0x00, 0x01, 0x00, 0xC0, 0x00, 0x50,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x71
							
	};
	uint8_t tlp_testdat[]  = {  0x00, 0x00, 0x00, 0x00,
		
								0x02,0x03,0x04,0x05,0x06 	};

	uint8_t tlp_isk [] = { 	0x01, 0x00 ,0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00,0x01
	};
	uint8_t tlp_new2 [] = { 0x04, 0x00, 0x00, 0x01, 
							0x00, 0xc0, 0x00, 0x0c, 
							0x03, 0x00, 0x00, 0x00 };
							
	uint8_t tlp_new3 [] = { 0x04, 0x00, 0x00, 0x01, 
							0x00, 0xc0, 0x00, 0x03, 
							0x03, 0x00, 0x00, 0x00 };

	uint8_t tlp_ans [] = { 	0x4a, 0x00, 0x00, 0x01,
							0x00, 0x00, 0x00, 0x04, 
							0x00, 0xc0, 0x00, 0x00, 
							0xde, 0x10, 0x22, 0x06  };
	uint8_t tlp_ans2 [] = { 	0x4a, 0x00, 0x00, 0x01,
							0x03, 0x00, 0x00, 0x04, 
							0x00, 0xc0, 0x00, 0x00, 
							0xde, 0x10, 0x22, 0x06  };



	for(uint16_t i = 0; i<100; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}

	for(uint16_t i = 0; i<300; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}

	tb->m_core->io_sim_state = 2; // Test Receive TLP
	//tcp_stack->Send_TLP(0, tlp_data, tlp_isk, sizeof(tlp_data));
	{

		/*uint16_t data_tl[] = {
				0xFB00, 0x0000, 0x0004, 0x0100, 0xc000, 0x0c00, 0x0003, 0x0000, 0x8b9f, 0xde9e, 0x00fd
		};
		uint8_t data_k[] = {
				0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0000, 0x00, 0x00, 0x01
		};*/

		uint16_t data_tl[] = {
				0x00FB, 0x4A00, 0x0000, 0x0001, 0x0000, 0x0004, 0x00C0, 0xDE00, 0x2210, 0x5606, 0xCD1A, 0xFD65,0x1CBC,0x1C1C
		};
		uint8_t data_k[] = {
				0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0000, 0x00, 0x00, 0x00, 0x02,0x03,0x03
		};

		/*uint16_t data_tl[] = {
				0xFB00, 0x0000, 0x004A, 0x0100, 0x0000, 0x0400, 0xC000, 0x0000, 0x10DE, 0x0622, 0x1A56, 0x65CD,0x00FD,0x1CBC, 0x1C1C
		};
		uint8_t data_k[] = {
				0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0000, 0x00, 0x00, 0x00, 0x00,0x01,0x03, 0x03
		};*/

		for(uint16_t i = 0; i<sizeof(data_tl); i++){
			tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
			tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

			tb->m_core->io_GTP_data_rx_charisk  = data_k[i];//tb2->m_core->io_GTP_data_tx_charisk;
			tb->m_core->io_GTP_data_rx_data  = data_tl[i];//tb2->m_core->io_GTP_data_tx_data;
			tb->tick();
			tb2->tick();
		}

					tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
			tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

			tb->m_core->io_GTP_data_rx_charisk  = 0;//tb2->m_core->io_GTP_data_tx_charisk;
			tb->m_core->io_GTP_data_rx_data  =0;//tb2->m_core->io_GTP_data_tx_data;
			tb->tick();
			tb2->tick();

			tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
			tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

			tb->m_core->io_GTP_data_rx_charisk  = 0;//tb2->m_core->io_GTP_data_tx_charisk;
			tb->m_core->io_GTP_data_rx_data  = 0;//tb2->m_core->io_GTP_data_tx_data;
			tb->tick();
			tb2->tick();
	}


	for(uint16_t i = 0; i<300; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}
	uint8_t tcp_ack0 [] = { 	0x02, 0x00, 0x00, 0x00 };

	tb->m_core->io_sim_state = 3; // Send TCP Packet
	tcp_stack->Send_Data(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_ack0, sizeof(tcp_ack0));

	for(uint16_t i = 0; i<300; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}

	tb->m_core->io_sim_state = 4; // Send Multi TLP
	send_initTLP(tb, tb2, tcp_stack);
/*
	tcp_stack->Send_Data(1, 0xC0A802D4,0x111213141516, 15000, 0x0, 0x6D1A5638, 0, tlp_testdat, sizeof(tlp_testdat));
	tcp_stack->Send_Data(0, 0xC0A802D5,0x111213141517, 15000, 0x15, 0x6D1A5638, 1, tlp_testdat,  sizeof(tlp_testdat));
*/
	tb->m_core->io_sim_state = 5; // Run Wait
	for(uint16_t i = 0; i<30000; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}
	tb->m_core->io_sim_state = 6; // Next ack check
	uint16_t tcp_ack1 [] = { 	0x5c00, 0x0000, 0x4900, 0xcc1e, 0xBCfd,0x1C1C };
	uint16_t tcp_isk1 [] = { 	0x2, 0x0, 0x0, 0x0, 0x3,0x3 };


	for(uint16_t i = 0;  i<sizeof(tcp_ack1); i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = tcp_isk1[i];
		tb->m_core->io_GTP_data_rx_data  = tcp_ack1[i];
		tb->tick();
		tb2->tick();
	}

	tb->m_core->io_sim_state = 7; // Wait ack check
	for(uint16_t i = 0; i<30; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}
	tb->m_core->io_sim_state = 8; // TLP Sending
	send_initTLP_back(tb, tb2, tcp_stack);
	tb->m_core->io_sim_state = 9; // Wait
	for(uint16_t i = 0; i<3000; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}


/*
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
*/
/*
  output        io_EthernetBus_busclock,
  output        io_EthernetBus_tx_strb,
  output [7:0]  io_EthernetBus_tx_data,
  output        io_EthernetBus_tx_run,
  input         io_EthernetBus_tx_ready,
  input  [47:0] io_EthernetBus_rx_info_srcMAC,
  input  [47:0] io_EthernetBus_rx_info_destMAC,
  input  [15:0] io_EthernetBus_rx_info_etype,
  input         io_EthernetBus_rx_strb,
  input  [7:0]  io_EthernetBus_rx_data,
  input         io_EthernetBus_rx_empty,
  input         io_EthernetBus_rx_last,
  input         io_EthernetBus_rx_error

*/

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
				CGREEN
				printf("Data Link is Up \n");
				CDEFAULT
				break;
				/*tb->m_core->io_GTP_data_rx_charisk  = 3;
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
				tb->tick();*/
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

	uint8_t arr[5] = {0x00,0x00,0x00,0x19,0xA4};
	uint8_t arr2[5] = {0x19,0x0F,0xFF,0x19,0xA4};
	uint8_t arr3[5] = {0x89,0xFF,0x85,0x19,0xA4};
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x50,arr));
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x94,arr3));
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x40,arr2));
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x50,arr));
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x40,arr));
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x40,arr,6));
	arr[3] = 0xF2; 
	arr[4] = 0xC3; 
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x50,arr,6));

	uint8_t arr5[] = {0x80,0x80,0x19,0x0F,0xFF};
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x80,arr5,sizeof(arr5)+1));
	//decode_Stream();
	/*uint8_t crc_tlp[] = {
		0x00, 0x2d, 0x44, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x03, 0x03, 0x00, 0x00, 0x04, 0x00, 0x00, 0xff, 0xff
	};
	printf("TLPCRC: 0x%X \n", tcp_stack->CalculateTlpCRC(crc_tlp,sizeof(crc_tlp)));*/

	//printf("TLPCRC: 0x%X \n", CalculateTlpCRC(tlp_arr,22));

	//uint8_t tlp_recv[] = { 0x0a,0x00,0x00,0x00,0x03,0x00,0x00,0x04,0x00,0xc0,0x00,0x00 };
	//uint8_t tlp_recv[] = { 0x44, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x0f, 0x03, 0x00, 0x00, 0x1c };
	//uint8_t tlp_recv[] = { 0x04, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x0f, 0x03, 0x00, 0x00, 0x1c };

	//printf("TLP_Recv:\n");
	//decode_TLP(tlp_recv,sizeof(tlp_recv));
	printf("LFSR: 0x%X \n",scramble_byte(0x00));
	printf("LFSR: 0x%X \n",scramble_byte(0x00));
	printf("LFSR: 0x%X \n",scramble_byte(0x80));
	scramble_byte(0x1BC);
	printf("LFSR: 0x%X \n",scramble_byte(0x50));
	
}

int scramble_byte(int inbyte)
{
	static int scrambit[16];
	static int bit[16];
	static int bit_out[16];
	static unsigned short lfsr = 0xffff; // 16 bit short for polynomial
	int i, outbyte;
	if (inbyte == 0x1BC) // if this is a comma
	{
		lfsr = 0xffff; // reset the LFSR
		lfsr_g = lfsr;
		return (0x1BC); // and return the same data
	}
	if (inbyte == 0x11C) // don't advance or encode on skip
		return (0x11C);
	for (i=0; i<16;i++) // convert LFSR to bit array for legibility
		bit[i] = (lfsr >> i) & 1;
	for (i=0; i<8; i++) // convert byte to be scrambled for legibility
		scrambit[i] = (inbyte >> i) & 1;
	// apply the xor to the data
	if (! (inbyte & 0x100) && // if not a KCODE, scramble the data
		! (TrainingSequence == true)) // and if not in the middle of
	{ // a training sequence
		scrambit[0] ^= bit[15];
		scrambit[1] ^= bit[14];
		scrambit[2] ^= bit[13];
		scrambit[3] ^= bit[12];
		scrambit[4] ^= bit[11];
		scrambit[5] ^= bit[10];
		scrambit[6] ^= bit[9];
		scrambit[7] ^= bit[8];
	}
	// Now advance the LFSR 8 serial clocks
	bit_out[ 0] = bit[ 8];
	bit_out[ 1] = bit[ 9];
	bit_out[ 2] = bit[10];
	bit_out[ 3] = bit[11] ^ bit[ 8];
	bit_out[ 4] = bit[12] ^ bit[ 9] ^ bit[ 8];
	bit_out[ 5] = bit[13] ^ bit[10] ^ bit[ 9] ^ bit[ 8];
	bit_out[ 6] = bit[14] ^ bit[11] ^ bit[10] ^ bit[ 9];
	bit_out[ 7] = bit[15] ^ bit[12] ^ bit[11] ^ bit[10];
	bit_out[ 8] = bit[ 0] ^ bit[13] ^ bit[12] ^ bit[11];
	bit_out[ 9] = bit[ 1] ^ bit[14] ^ bit[13] ^ bit[12];
	bit_out[10] = bit[ 2] ^ bit[15] ^ bit[14] ^ bit[13];
	bit_out[11] = bit[ 3] ^ bit[15] ^ bit[14];
	bit_out[12] = bit[ 4] ^ bit[15];
	bit_out[13] = bit[ 5];
	bit_out[14] = bit[ 6];
	bit_out[15] = bit[ 7];
	lfsr = 0;
	for (i=0; i <16; i++) // convert the LFSR back to an integer
		lfsr += (bit_out[i] << i);
	outbyte = 0;
	for (i= 0; i<8; i++) // convert data back to an integer
		outbyte += (scrambit[i] << i);
	//printf("LFSR_State = 0x%X \n",lfsr);
	lfsr_g = lfsr;
	return outbyte;
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
/* Low_latency.vcd -> Ethernet 1GBit
void decode_Stream(){
	uint8_t tlp_recv[] = {0x04,0x00,0x00,0x01,0x00,0xC0,0x00,0x0F,0x03,0x00,0x00,0x00};
	uint8_t tlp_send[] = {};
	uint8_t dllp[4];

	printf("DLLP_Recv:\n")
	dllp[0] = 0x90;
	dllp[1] = 0x08;
	dllp[2] = 0x00;
	dllp[3] = 0x02;
	decode_DLLP(dllp,sizeof(dllp));

	printf("DLLP_Recv:\n")
	dllp[0] = 0x80;
	dllp[1] = 0x04;
	dllp[2] = 0x00;
	dllp[3] = 0x40;
	decode_DLLP(dllp,sizeof(dllp));

	printf("DLLP_Send:\n")
	dllp[0] = 0xA0;
	dllp[1] = 0x00;
	dllp[2] = 0x4F;
	dllp[3] = 0xFF;
	decode_DLLP(dllp,sizeof(dllp));
	
	printf("DLLP_Recv:\n")
	dllp[0] = 0x90;
	dllp[1] = 0x08;
	dllp[2] = 0x00;
	dllp[3] = 0x02;
	decode_DLLP(dllp,sizeof(dllp));
	
	printf("DLLP_Recv:\n")
	dllp[0] = 0x80;
	dllp[1] = 0x04;
	dllp[2] = 0x00;
	dllp[3] = 0x40;
	decode_DLLP(dllp,sizeof(dllp));

	printf("DLLP_Send:\n")
	dllp[0] = 0x90;
	dllp[1] = 0x00;
	dllp[2] = 0x4F;
	dllp[3] = 0xFF;
	decode_DLLP(dllp,sizeof(dllp));

	printf("DLLP_Recv:\n")
	dllp[0] = 0x90;
	dllp[1] = 0x08;
	dllp[2] = 0x00;
	dllp[3] = 0x02;
	decode_DLLP(dllp,sizeof(dllp));
	
	printf("DLLP_Recv:\n")
	dllp[0] = 0x80;
	dllp[1] = 0x04;
	dllp[2] = 0x00;
	dllp[3] = 0x40;
	decode_DLLP(dllp,sizeof(dllp));
	
	printf("DLLP_Send:\n")
	dllp[0] = 0x80;
	dllp[1] = 0x00;
	dllp[2] = 0x4F;
	dllp[3] = 0xFF;
	decode_DLLP(dllp,sizeof(dllp));
	
	printf("DLLP_Recv:\n")
	dllp[0] = 0x90;
	dllp[1] = 0x08;
	dllp[2] = 0x00;
	dllp[3] = 0x02;
	decode_DLLP(dllp,sizeof(dllp));
	
	printf("DLLP_Recv:\n")
	dllp[0] = 0x80;
	dllp[1] = 0x04;
	dllp[2] = 0x00;
	dllp[3] = 0x40;
	decode_DLLP(dllp,sizeof(dllp));
		
	printf("DLLP_Send:\n")
	dllp[0] = 0xA0;
	dllp[1] = 0x00;
	dllp[2] = 0x4F;
	dllp[3] = 0xFF;
	decode_DLLP(dllp,sizeof(dllp));

	printf("DLLP_Recv:\n")
	dllp[0] = 0x90;
	dllp[1] = 0x08;
	dllp[2] = 0x00;
	dllp[3] = 0x02;
	decode_DLLP(dllp,sizeof(dllp));
	
	printf("DLLP_Recv:\n")
	dllp[0] = 0x80;
	dllp[1] = 0x04;
	dllp[2] = 0x00;
	dllp[3] = 0x40;
	decode_DLLP(dllp,sizeof(dllp));
	
	printf("TLP_Recv Seq = 1:\n")
	decode_DLLP(tlp_recv,sizeof(tlp_recv));
			
	printf("DLLP_Send:\n")
	dllp[0] = 0x00;
	dllp[1] = 0x00;
	dllp[2] = 0x00;
	dllp[3] = 0x01;
	decode_DLLP(dllp,sizeof(dllp));

}*/

void wait_Clock(LTSSM_TB *tb, LTSSM_TB *tb2, uint32_t n){
	for(uint32_t i = 0; i<n; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}
}

void send_initTLP_back(LTSSM_TB *tb, LTSSM_TB *tb2, TCP_Bus *tcp_stack ){
	
	/*uint16_t data[] = {
			0xFB00, 0x0100, 0x004A, 0x0100, 0x0000, 0x0400, 0xC000, 0x0000, 0x10DE, 0x0622, 0x1A56, 0x65CD,0x00FD
	};*/

	uint8_t data[] = {
		0x00, 0x01, 0x4A, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0xC0, 0x00, 0x00, 0xDE, 0x10, 0x22, 0x06
	};
	uint8_t data_k[] = {
			0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0000, 0x00, 0x00, 0x00, 0x00,0x01
	};
	uint8_t tcp_seq0 [] = { 	
		0x02, 0x00, 0x00, 0x01
	};
	for(uint8_t x = 0; x < 100; x++){
		/*for(uint16_t i = 0; i<sizeof(data); i++){
			tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
			tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

			tb->m_core->io_GTP_data_rx_charisk  = data_k[i];//tb2->m_core->io_GTP_data_tx_charisk;
			tb->m_core->io_GTP_data_rx_data  = data[i];//tb2->m_core->io_GTP_data_tx_data;
			tb->tick();
			tb2->tick();
		}*/
		
		wait_Clock(tb, tb2, 300);
		if( (x%2) == 0){
			tcp_stack->Send_TLP_Checksum(0, data, sizeof(data)-1);
		}else{
			tcp_stack->Send_TLP_Checksum(0, data, sizeof(data));
		}
		tcp_stack->Send_Data(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq0, sizeof(tcp_seq0));
		data[1] = data[1] + 1;
		tcp_seq0[3] = tcp_seq0[3] + 1;
	}
	
}

void send_initTLP(LTSSM_TB *tb, LTSSM_TB *tb2, TCP_Bus *tcp_stack ){

	uint8_t tcp_seq0 [] = { 	
		0x00, 0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	tcp_stack->Send_Data(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq0, sizeof(tcp_seq0));
	wait_Clock(tb, tb2, 300);


	uint8_t tcp_seq1 [] = { 	
		0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x00
	};
	tcp_stack->Send_Data2Pad(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq1, sizeof(tcp_seq1));
	wait_Clock(tb, tb2, 300);


	uint8_t tcp_seq2 [] = { 	
		0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00
	};
	tcp_stack->Send_Data2Pad(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq2, sizeof(tcp_seq2));
	wait_Clock(tb, tb2, 300);


	uint8_t tcp_seq3 [] = { 	
		0x00, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x04, 0x03, 0x00, 0x00, 0x0c
	};
	tcp_stack->Send_Data2Pad(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq3, sizeof(tcp_seq3));
	wait_Clock(tb, tb2, 300);


	uint8_t tcp_seq4 [] = { 	
		0x00, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x00
	};
	tcp_stack->Send_Data2Pad(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq4, sizeof(tcp_seq4));
	wait_Clock(tb, tb2, 300);


	uint8_t tcp_seq5 [] = { 	
		0x00, 0x00, 0x00, 0x05, 0x04, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00
	};
	tcp_stack->Send_Data2Pad(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq5, sizeof(tcp_seq5));
	wait_Clock(tb, tb2, 300);


	uint8_t tcp_seq6 [] = { 	
		0x00, 0x00, 0x00, 0x06, 0x04, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x08
	};
	tcp_stack->Send_Data2Pad(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq6, sizeof(tcp_seq6));
	wait_Clock(tb, tb2, 300);


	uint8_t tcp_seq7 [] = { 	
		0x00, 0x00, 0x00, 0x07, 0x44, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x0f, 0x03, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x8c, 0x3a, 0x8c
	};
	tcp_stack->Send_Data2Pad(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq7, sizeof(tcp_seq7));
	wait_Clock(tb, tb2, 300);


	uint8_t tcp_seq8 [] = { 	
		0x00, 0x00, 0x00, 0x08, 0x44, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x0f, 0x03, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00
	};
	tcp_stack->Send_Data2Pad(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq8, sizeof(tcp_seq8));
	wait_Clock(tb, tb2, 300);


	uint8_t tcp_seq9 [] = { 	
		0x00, 0x00, 0x00, 0x09, 0x04, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x00
	};
	tcp_stack->Send_Data2Pad(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq9, sizeof(tcp_seq9));
	wait_Clock(tb, tb2, 300);


	uint8_t tcp_seq10 [] = { 	
		0x00, 0x00, 0x00, 0x0a, 0x04, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00
	};
	tcp_stack->Send_Data2Pad(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq10, sizeof(tcp_seq10));
	//wait_Clock(tb, tb2, 300);
	/*for(uint32_t i = 0; i<300; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->m_core->io_GTP_data_rx_data  = 0;
		tb->tick();
		tb2->tick();
	}*/
	/*for(uint32_t i = 0; i<30000; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->m_core->io_GTP_data_rx_data  = 0;
		tb->tick();
		tb2->tick();
	}*/
	for(uint8_t i = 0; i<200; i++){
		tcp_seq10[3] = 0x0b+i;
			//tcp_stack->Send_Data2Pad(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq10, sizeof(tcp_seq10));
			if(i%2 == 0){
				tcp_stack->Send_Data(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq10, sizeof(tcp_seq10));
			}else{
				tcp_stack->Send_Data(0, 0xC0A802D5,0x111213141517, 15000, 0x0, 0x6D1A5638, 0, tcp_seq10, sizeof(tcp_seq10)-1);
			}
			
			wait_Clock(tb, tb2, 300);
			/*if(i >= 1){
				for(uint32_t i = 0; i<300; i++){
					tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
					tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

					tb->m_core->io_GTP_data_rx_charisk  = 0;
					tb->m_core->io_GTP_data_rx_data  = 0;
					tb->tick();
					tb2->tick();
				}
			}else{
				wait_Clock(tb, tb2, 300);
			}*/
			

	}


	/*uint16_t acktest[] = {0x005c,0x0000,0xf90a,0xfd88,0xfd88};

	uint8_t acktest_k[] = {0x01,0x00,0x00,0x02,0x02};

	for(uint32_t i = 0; i<5; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = acktest_k[i];
		tb->m_core->io_GTP_data_rx_data  = acktest[i];
		tb->tick();
		tb2->tick();
	}

	for(uint32_t i = 0; i<300; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->m_core->io_GTP_data_rx_data  = 0;
		tb->tick();
		tb2->tick();
	}

	uint16_t acktest0[] = {0x5c00,0x0000,0x0a00,0x88f9,0x00fd};

	uint8_t acktest0_k[] = {0x02,0x00,0x00,0x00,0x01};
	for(uint32_t i = 0; i<5; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = acktest0_k[i];
		tb->m_core->io_GTP_data_rx_data  = acktest0[i];
		tb->tick();
		tb2->tick();
	}*/

	for(uint32_t i = 0; i<300; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = 0;
		tb->m_core->io_GTP_data_rx_data  = 0;
		tb->tick();
		tb2->tick();
	}
}

void decode_Stream(){
	uint8_t tlp_recv[] = {0x04,0x00,0x00,0x01,0x00,0xC0,0x00,0x0F,0x03,0x00,0x00,0x00};
	uint8_t tlp_send[] = {0x4A,0x00,0x00,0x01,0x00,0x00,0x00,0x04,0x00,0xC0,0x00,0x00,0xDE,0x10,0x22,0x06};
	uint8_t dllp[4];
	CGREEN
	printf("DLLP_Recv:\n");
	CDEFAULT
	dllp[0] = 0x90;
	dllp[1] = 0x08;
	dllp[2] = 0x00;
	dllp[3] = 0x02;
	decode_DLLP(dllp,sizeof(dllp));

	CGREEN
	printf("DLLP_Recv:\n");
	CDEFAULT
	dllp[0] = 0x80;
	dllp[1] = 0x04;
	dllp[2] = 0x00;
	dllp[3] = 0x40;
	decode_DLLP(dllp,sizeof(dllp));

	CGREEN
	printf("DLLP_Send:\n");
	CDEFAULT
	dllp[0] = 0xA0;
	dllp[1] = 0x00;
	dllp[2] = 0x4F;
	dllp[3] = 0xFF;
	decode_DLLP(dllp,sizeof(dllp));

	CRED
	printf("TLP_Recv Seq= 1:\n");
	CDEFAULT
	decode_TLP(tlp_recv,sizeof(tlp_recv));

	CGREEN
	printf("DLLP_Send:\n");
	CDEFAULT
	dllp[0] = 0x00;
	dllp[1] = 0x00;
	dllp[2] = 0x00;
	dllp[3] = 0x01;
	decode_DLLP(dllp,sizeof(dllp));

	CRED	
	printf("TLP_Send Seq = 0:\n");
	CDEFAULT
	decode_TLP(tlp_send,sizeof(tlp_send));

	CGREEN
	printf("DLLP_Recv:\n");
	CDEFAULT
	dllp[0] = 0x00;
	dllp[1] = 0x00;
	dllp[2] = 0x00;
	dllp[3] = 0x00;
	decode_DLLP(dllp,sizeof(dllp));

	CGREEN
	printf("DLLP_Recv:\n");
	CDEFAULT
	dllp[0] = 0x90;
	dllp[1] = 0x08;
	dllp[2] = 0x00;
	dllp[3] = 0x02;
	decode_DLLP(dllp,sizeof(dllp));

	CGREEN
	printf("DLLP_Recv:\n");
	CDEFAULT
	dllp[0] = 0x80;
	dllp[1] = 0x04;
	dllp[2] = 0x00;
	dllp[3] = 0x40;
	decode_DLLP(dllp,sizeof(dllp));

	CGREEN
	printf("DLLP_Send:\n");
	CDEFAULT
	dllp[0] = 0x90;
	dllp[1] = 0x00;
	dllp[2] = 0x4F;
	dllp[3] = 0xFF;
	decode_DLLP(dllp,sizeof(dllp));

	CGREEN
	printf("DLLP_Recv:\n");
	CDEFAULT
	dllp[0] = 0x90;
	dllp[1] = 0x08;
	dllp[2] = 0x00;
	dllp[3] = 0x02;
	decode_DLLP(dllp,sizeof(dllp));

	CGREEN
	printf("DLLP_Recv:\n");
	CDEFAULT
	dllp[0] = 0x80;
	dllp[1] = 0x04;
	dllp[2] = 0x00;
	dllp[3] = 0x40;
	decode_DLLP(dllp,sizeof(dllp));

	CGREEN
	printf("DLLP_Send:\n");
	CDEFAULT
	dllp[0] = 0x80;
	dllp[1] = 0x00;
	dllp[2] = 0x4F;
	dllp[3] = 0xFF;
	decode_DLLP(dllp,sizeof(dllp));

	CGREEN
	printf("DLLP_Recv:\n");
	CDEFAULT
	dllp[0] = 0x90;
	dllp[1] = 0x08;
	dllp[2] = 0x00;
	dllp[3] = 0x02;
	decode_DLLP(dllp,sizeof(dllp));

	CGREEN
	printf("DLLP_Recv:\n");
	CDEFAULT
	dllp[0] = 0x80;
	dllp[1] = 0x04;
	dllp[2] = 0x00;
	dllp[3] = 0x40;
	decode_DLLP(dllp,sizeof(dllp));
	
	CGREEN
	printf("DLLP_Send:\n");
	CDEFAULT
	dllp[0] = 0xA0;
	dllp[1] = 0x00;
	dllp[2] = 0x4F;
	dllp[3] = 0xFF;
	decode_DLLP(dllp,sizeof(dllp));

	
	CGREEN
	printf("DLLP_Recv:\n");
	CDEFAULT
	dllp[0] = 0x90;
	dllp[1] = 0x08;
	dllp[2] = 0x00;
	dllp[3] = 0x02;
	decode_DLLP(dllp,sizeof(dllp));

	CGREEN
	printf("DLLP_Recv:\n");
	CDEFAULT
	dllp[0] = 0x80;
	dllp[1] = 0x04;
	dllp[2] = 0x00;
	dllp[3] = 0x40;
	decode_DLLP(dllp,sizeof(dllp));	

	CGREEN
	printf("DLLP_Send:\n");
	CDEFAULT
	dllp[0] = 0x80;
	dllp[1] = 0x00;
	dllp[2] = 0x4F;
	dllp[3] = 0xFF;
	decode_DLLP(dllp,sizeof(dllp));
}

void decode_DLLP(uint8_t* data, uint8_t len){
	uint8_t type = 0;
	uint16_t seq = 0;
	uint16_t headerFC, dataFC = 0;
	for (uint8_t i = 0; i<len; i++){
		switch (i)
		{
		case 0:
			if(data[i] == 0){	// Ack
				printf("Type: Ack \n");
				type = 0;
			}
			if(data[i] == 0x10){	// NAck
				printf("Type: NAck \n");
				type = 1;
			}
			if( (data[i]&0xC8) == 0x40){	// InitFC1
				type = 2;
				printf("Format: InitFC1\n");
				switch(data[i]&0x30){
					case 0: // Posted
						printf("Type: Posted\n");
						break;
					case 0x10: // Non Posted
						printf("Type: Non Posted\n");
						break;
					case 0x20: // Completed
						printf("Type: Completed\n");
						break;
				}
				printf("VC: %i\n", data[i]&0x03);
			}
			if( (data[i]&0xC8) == 0xC0){	// InitFC2
				type = 3;
				printf("Format: InitFC2\n");
				switch(data[i]&0x30){
					case 0: // Posted
						printf("Type: Posted\n");
						break;
					case 0x10: // Non Posted
						printf("Type: Non Posted\n");
						break;
					case 0x20: // Completed
						printf("Type: Completed\n");
						break;
				}
				printf("VC: %i\n", data[i]&0x03);
			}
			if( (data[i]&0xC8) == 0x80){	// UpdateFC
				type = 4;
				printf("Format: UpdateFC\n");
				switch(data[i]&0x30){
					case 0: // Posted
						printf("Type: Posted\n");
						break;
					case 0x10: // Non Posted
						printf("Type: Non Posted\n");
						break;
					case 0x20: // Completed
						printf("Type: Completed\n");
						break;
				}
				printf("VC: %i\n", data[i]&0x03);
			}
			if( (data[i]&0xF8) == 0x20){	// PM DLLP
				type = 5;
				printf("Type: PM\n");
			}
			if( (data[i]) == 0x30){	// Vendor
				type = 5;
				printf("Type: PM\n");
			}
			break;
		case 1:
			switch(type){
				case 2:	// Init Update FC
				case 3:
				case 4:
					headerFC = (data[i]&0x3F) << 2;
					break;

			}
			break;
		case 2:
			switch(type){
				case 2:	// Init Update FC
				case 3:
				case 4:
					headerFC |= (data[i]&0xC0) >> 6;
					dataFC = (data[i]&0x0F)<<8;
					printf("Header FC: %i\n", headerFC);
					break;
				case 0:	// Seq number (Ack Nak)
				case 1:
					seq = ((data[i]&0x0F)<<8);
			}
			break;
		case 3:
			switch(type){
				case 2:	// Init Update FC
				case 3:
				case 4:
					dataFC |= data[i];
					printf("Data FC: %i\n", dataFC);
					break;
				case 0:	// Seq number (Ack Nak)
				case 1:
					seq |= data[i];
					printf("Seq Number: %i\n", seq);
			}
			break;
		}
	}
}

void decode_type(uint8_t format, uint8_t type){
	if( (format & 0x03) == 0){
		printf("3DW Header, no data \n");
	}else if( (format & 0x03) == 1){
		printf("4DW Header, no data \n");
	}else if( (format & 0x03) == 2){
		printf("3DW Header, with data \n");
	}else if( (format & 0x03) == 3){
		printf("4DW Header, with data \n");
	}else if( (format & 0x03) == 4){
		printf("TLP Prefix \n");
	}

	if ( ((format & 0x06) == 0) && ((type & 0x1F) == 0) ){	// MRd
		printf("MRd Non-Posted\n");
	}
	if ( ((format & 0x06) == 0) && ((type & 0x1F) == 1) ){	// MRdLk
		printf("MRdLk Non-Posted\n");
	}
	if ( ((format & 0x06) == 1) && ((type & 0x1F) == 0) ){	// MWr
		printf("MWr Posted\n");
	}
	if ( ((format & 0x07) == 0) && ((type & 0x1F) == 2) ){	// IORd
		printf("IORd Non-Posted\n");
	}
	if ( ((format & 0x07) == 2) && ((type & 0x1F) == 2) ){	// IOWr
		printf("IOWr Non-Posted\n");
	}
	if ( ((format & 0x07) == 0) && ((type & 0x1F) == 4) ){	// CfgRd0
		printf("CfgRd0 Non-Posted\n");
	}
	if ( ((format & 0x07) == 2) && ((type & 0x1F) == 4) ){	// CfgWr0
		printf("CfgWr0 Non-Posted\n");
	}
	if ( ((format & 0x07) == 0) && ((type & 0x1F) == 5) ){	// CfgRd1
		printf("CfgRd1 Non-Posted\n");
	}
	if ( ((format & 0x07) == 2) && ((type & 0x1F) == 5) ){	// CfgWr1
		printf("CfgWr1 Non-Posted\n");
	}
	if ( ((format & 0x07) == 0) && ((type & 0x1F) == 27) ){	// TCfgRd
		printf("TCfgRd Non-Posted\n");
	}
	if ( ((format & 0x07) == 2) && ((type & 0x1F) == 27) ){	// TCfgWr
		printf("TCfgWr Non-Posted\n");
	}
	if ( ((format & 0x07) == 1) && ((type & 0x18) == 0x10) ){	// Msg
	
		printf("Msg Posted ");
		switch ((type&0x07))
		{
			case 0:
				printf( "Routed to Root Complex\n");
				break;
			case 1:
				printf( "Routed by Address\n");
				break;
			case 2:
				printf( "Routed by ID\n");
				break;
			case 3:
				printf( "Broadcast from Root Complex\n");
				break;
			case 4:
				printf( "Local - Terminate\n");
				break;
			case 5:
				printf( "Gathered and routed to Root Complex\n");
				break;
			default:
				printf( "Reserved - Terminate\n");
				break;
		}
	}
	if ( ((format & 0x07) == 3) && ((type & 0x18) == 0x10) ){	// MsgD
		printf("MsgD Posted");
		switch ((type&0x07))
		{
			case 0:
				printf( "Routed to Root Complex\n");
				break;
			case 1:
				printf( "Routed by Address\n");
				break;
			case 2:
				printf( "Routed by ID\n");
				break;
			case 3:
				printf( "Broadcast from Root Complex\n");
				break;
			case 4:
				printf( "Local - Terminate\n");
				break;
			case 5:
				printf( "Gathered and routed to Root Complex\n");
				break;
			default:
				printf( "Reserved - Terminate\n");
				break;
		}
	}
	if ( ((format & 0x07) == 0) && ((type & 0x1F) == 10) ){	// Cpl
		printf("Cpl Completion\n");
	}
	if ( ((format & 0x07) == 2) && ((type & 0x1F) == 10) ){	// CplD
		printf("CplD Completion\n");
	}
	if ( ((format & 0x07) == 0) && ((type & 0x1F) == 11) ){	// CplLk
		printf("CplLk Completion\n");
	}
	if ( ((format & 0x07) == 2) && ((type & 0x1F) == 11) ){	// CplDLk
		printf("CplDLk Completion\n");
	}
	if ( ((format & 0x06) == 2) && ((type & 0x1F) == 12) ){	// FetchAdd
		printf("FetchAdd Completion\n");
	}
	if ( ((format & 0x06) == 2) && ((type & 0x1F) == 13) ){	// Swap
		printf("Swap Completion\n");
	}
	if ( ((format & 0x06) == 2) && ((type & 0x1F) == 14) ){	// CAS
		printf("CAS Completion\n");
	}
	if ( ((format & 0x07) == 4) && ((type & 0x10) == 0) ){	// LPrfx
		printf("LPrfx\n");
	}
	if ( ((format & 0x07) == 4) && ((type & 0x10) == 0x10) ){	// EPrfx
		printf("EPrfx\n");
	}
}
/*
	Data Formats p. 75

	-> 0 = Memory Requests
	-> 1 = IO Requests
	-> 2 = Config Requests
	-> 3 = Completion
	-> 4 = TLP Prefix
	-> 5 = Message Request
*/
uint8_t tlp_data_format(uint8_t format, uint8_t type){

	if ( ((format & 0x06) == 0) && ((type & 0x1F) == 0) ||
		((format & 0x06) == 0) && ((type & 0x1F) == 1) ||
		((format & 0x06) == 1) && ((type & 0x1F) == 0) ||
		((format & 0x06) == 2) && ((type & 0x1F) == 12)||
		((format & 0x06) == 2) && ((type & 0x1F) == 13)||
		((format & 0x06) == 2) && ((type & 0x1F) == 14)
	){	// Memory Requests and Atomics
		return 0;
	}

	if ( ((format & 0x07) == 0) && ((type & 0x1F) == 2) ||
		((format & 0x07) == 2) && ((type & 0x1F) == 2)
	){	// IO Requests
		return 1;
	}

	if ( ((format & 0x07) == 0) && ((type & 0x1F) == 4) ||
		((format & 0x07) == 2) && ((type & 0x1F) == 4)	||
		((format & 0x07) == 0) && ((type & 0x1F) == 5)	||
		((format & 0x07) == 2) && ((type & 0x1F) == 5) 	||
		((format & 0x07) == 0) && ((type & 0x1F) == 27) ||
		((format & 0x07) == 2) && ((type & 0x1F) == 27) 
	){	// Config Requests
		return 2;
	}

	if ( ((format & 0x07) == 1) && ((type & 0x18) == 0x10) ||
		((format & 0x07) == 3) && ((type & 0x18) == 0x10)
	){	// Message Request
		return 5;
	}
	
	if ( ((format & 0x07) == 0) && ((type & 0x1F) == 10) ||
		((format & 0x07) == 2) && ((type & 0x1F) == 10) ||
		((format & 0x07) == 0) && ((type & 0x1F) == 11) ||
		((format & 0x07) == 2) && ((type & 0x1F) == 11)
	){	// Completion
		return 3;
	}

	if ( ((format & 0x07) == 4) && ((type & 0x10) == 0) ||
		((format & 0x07) == 4) && ((type & 0x10) == 0x10)
	
	){	// TLP Prefix
		return 4;
	}
	return 0;
}

void decode_Message_Code(uint8_t code, uint8_t r){
	switch(code){
		case 0x10:	// LTR Message r(2:0) = 4
			printf("Message: LTR\n");
			break;
		
		case 0x41: // Ignored Messages  r(2:0) = 4
		case 0x43:	
		case 0x40:	
		case 0x45:	
		case 0x47:	
		case 0x44:	
		case 0x48:	
			printf("Message: Ignored\n");
			break;

		case 0x7E:	// Vendor Defined
		case 0x7F:	
			printf("Message: Vendor Defined\n");
			break;

		case 0x50:	// Set Slot Power Limit r(2:0) = 4
			printf("Message: Set Slot Power Limit\n");
			break;

		case 0x00:	// Unlock r(2:0) = 3
			printf("Message: Unlock\n");
			break;

		case 0x30:	// Error Correctable
			printf("Message: Correctable PCIe Error\n");
			break;

		case 0x31:	// Error Non Fatal
			printf("Message: Non Fatal PCIe Error\n");
			break;

		case 0x33:	// Error Fatal
			printf("Message: Fatal PCIe Error\n");
			break;

		case 0x14:	// PM Active State Nak  r(2:0) = 4
			printf("Message: PM Active State Nak\n");
			break;

		case 0x18:	// PM PME  r(2:0) = 0
			printf("Message: PM PME\n");
			break;

		case 0x19:	// PME Turn Off  r(2:0) = 3
			printf("Message: PM PME Turn Off\n");
			break;

		case 0x1B:	// PME TO Ack  r(2:0) =5
			printf("Message: PM PME TO Ack\n");
			break;

		case 0x24:	// Deassert Int A r(2:0) = 4
			printf("Message: Deassert INT B\n");
			break;

		case 0x25:	// Deassert Int B r(2:0) = 4
			printf("Message: Deassert INT B\n");
			break;

		case 0x26:	// Deassert Int C r(2:0) = 4
			printf("Message: Deassert INT B\n");
			break;

		case 0x27:	// Deassert Int D r(2:0) = 4
			printf("Message: Deassert INT B\n");
			break;

		case 0x20:	// Assert Int A r(2:0) = 4
			printf("Message: Assert INT B\n");
			break;

		case 0x21:	// Assert Int B r(2:0) = 4
			printf("Message: Assert INT B\n");
			break;

		case 0x22:	// Assert Int C r(2:0) = 4
			printf("Message: Assert INT B\n");
			break;

		case 0x23:	// Assert Int D r(2:0) = 4
			printf("Message: Assert INT B\n");
			break;
	}
}

void decode_TLP(uint8_t* data, uint8_t len){
	if (len >= 12){
		uint8_t format,type, addr64, data_format;
		uint16_t Length;
		uint16_t Requester, Completer, ByteCount;
		uint64_t Address;
		uint8_t diff = 0;
		for(uint8_t i = 0; i<len ; i++)
		{
			switch(i){
				case 0: // FMT and Type
					format = (data[i]&0xE0)>>5;
					type = (data[i]&0x1F);
					addr64 = format & 0x01;	// 64 Bit addressing -> 4 DWord header
					if (addr64){
						diff = 16;
					}else{
						diff = 12;
					}
					decode_type(format,type);
					data_format = tlp_data_format(format,type);
					break;
				case 1: // Traffic Class, Attributes, Hints
					printf("Traffic Class: %i\n", ((data[i]&0x70)>>4) );
					printf("Attribute: %i\n", ((data[i]&0x04)>>2) );
					printf("TLP Hints: %i\n", ((data[i]&0x01)) );
					break;
				case 2: // Digest, Poisoned, More Attributes
					printf("TLP Digest: %i, ", ((data[i]&0x80)>>7) );
					if (((data[i]&0x80)>>7) == 0){
						printf("No Digest\n");
					}else{
						printf("With Digest\n");
					}
					printf("TLP Poisoned: %i\n", ((data[i]&0x40)>>6) );
					printf("Attribute2: %i\n", ((data[i]&0x30)>>4) );
					switch(((data[i]&0x0C)>>2)){
						case 0:
							printf("Adress Type: Untranslated\n"  );
							break;
						case 1:
							printf("Adress Type: Translation Request\n"  );
							break;
						case 2:
							printf("Adress Type: Translated\n"  );
							break;
						case 3:
							printf("Adress Type: Reserved\n"  );
							break;
					}
					Length = ((uint16_t)data[i]&0x03)<<8;
					break;
				case 3: // Length
					Length |= data[i];
					printf("Length: %i DW\n", Length);
					break;
				case 4: //  Vary with type
					switch (data_format)
					{
						case 0: // Requester ID on all Requests
						case 1:
						case 2:
						case 5:
							Requester = (uint16_t)data[i]<<8;
							break;
						case 3: // Completer ID on Completion
							Completer = (uint16_t)data[i]<<8;
							break;
					}
					break;
				case 5: //  Vary with type
					switch (data_format)
					{
						case 0: // Requester ID on all Requests
						case 1:
						case 2:
						case 5:
							Requester |= data[i];
							printf("Requester ID: %i\n", Requester);
							break;
						case 3: // Completer ID on Completion
							Completer |= data[i];
							printf("Completer ID: %i\n", Completer);
							break;
					}
					break;
				case 6: //  Vary with type
					switch (data_format)
					{
						case 0: // Tag on all Requests
						case 1:
						case 2:
						case 5:
							printf("Tag: %i\n", data[i]);
							break;
						case 3: //  on Completion
							switch(((data[i]&0xE0)>>5)){
								case 0:
									printf("Completion Status: Succesful\n");
									break;
								case 1:
									printf("Completion Status: Unsupported Request\n");
									break;
								case 2:
									printf("Completion Status: Config Request Retry\n");
									break;
								case 3:
									printf("Completion Status: Completer Abort\n");
									break;
								default:
									printf("Completion Status: Reserved\n");
									break;
							}
							printf("Byte Count Modified: %i\n", ((data[i]&0x10)>>4) );
							ByteCount = (uint16_t)data[i]<<8;
							break;
					}
					break;
				case 7: // Byte Enables
					switch (data_format)
					{
						case 0: // Tag on Config, Mem and IO Requests
						case 1:
						case 2:
							/*switch(((data[i]&0xF0)>>4)){
								case 1:
									printf("Byte 0 in Last DW is valid\n"  );
									break;
								case 2:
									printf("Byte 1 in Last DW is valid\n"  );
									break;
								case 4:
									printf("Byte 2 in Last DW is valid\n"  );
									break;
								case 8:
									printf("Byte 3 in Last DW is valid\n"  );
									break;
							}
							switch(((data[i]&0x0F))){
								case 1:
									printf("Byte 0 in First DW is valid\n"  );
									break;
								case 2:
									printf("Byte 1 in First DW is valid\n"  );
									break;
								case 4:
									printf("Byte 2 in First DW is valid\n"  );
									break;
								case 8:
									printf("Byte 3 in First DW is valid\n"  );
									break;
							}
							break;*/
							printf("Byte En Last DW: 0x%X\n", ((data[i]&0xF0)>>4));
							printf("Byte En First DW: 0x%X\n", ((data[i]&0x0F)));
							break;
						case 3: //  on Completion
							ByteCount |= data[i];
							printf("ByteCount: %i\n", ByteCount);
							break;
						case 5:
							printf("Message Code: 0x%X\n", data[i]);
							decode_Message_Code(data[i], 0);
							break;
					}
					break;
				case 8: //  Vary with type
					switch (data_format)
					{
						case 0: // Memory and IO
						case 1:
							if (addr64){
								Address = (uint64_t)data[i]<<56;
							}else{
								Address = (uint64_t)data[i]<<24;
							}
							break;
						case 2: // Configuration
							printf("Bus Number: %i\n", data[i]);
							break;
						case 3: // Completion
							Requester = (uint16_t)data[i]<<8;
							break;
					}
					break;
				case 9: //  Vary with type
					switch (data_format)
					{
						case 0: // Memory and IO
						case 1: 
							if (addr64){
								Address |= (uint64_t)data[i]<<48;
							}else{
								Address |= (uint64_t)data[i]<<16;
							}
							break;
						case 2: // Configuration
							printf("Device Number: %i\n", ((data[i]&0xF8) >>3) );
							printf("Func Number: %i\n", data[i]&0x07 );
							break;
						case 3: // Completion
							Requester |= data[i];
							printf("Requester ID: %i\n", Requester);
							break;
					}
					break;
				case 10: //  Vary with type
					switch (data_format)
					{
						case 0: // Memory and IO
						case 1:
							if (addr64){
								Address |= (uint64_t)data[i]<<40;
							}else{
								Address |= (uint64_t)data[i]<<8;
							}
							break;
						case 2: // Configuration
							printf("Ext Reg Number: %i\n", (data[i]&0x0F) );
							break;
						case 3: // Completion
							printf("Tag: %i\n", data[i]);
							break;
					}
					break;
				case 11: //  Vary with type
					switch (data_format)
					{
						case 0: // Memory and IO
						case 1:
							if (addr64){
								Address |= (uint64_t)data[i]<<32;
							}else{
								Address |= (uint64_t)data[i];
								printf("Adress: 0x%lX\n", Address);
							}
							break;
						case 2: // Configuration
							printf("Reg Number: %i\n", ((data[i]&0xFC)>>2) );
							break;
						case 3: // Completion
							printf("Lower Address: %i\n", data[i]&0x7F);
							break;
					}
					break;
				case 12: //  Vary with type
					if (addr64){	// 4 DW
						
					}else{
						printf("Data %i : 0x%X\n" , (i-diff),data[i]);
					}
					break;
				case 13: //  Vary with type
					if (addr64){	// 4 DW
						
					}else{
						printf("Data %i : 0x%X\n" , (i-diff),data[i]);
					}
					break;
				case 14: //  Vary with type
					if (addr64){	// 4 DW
						
					}else{
						printf("Data %i : 0x%X\n" , (i-diff),data[i]);
					}
					break;
				case 15: //  Vary with type
					if (addr64){	// 4 DW
						
					}else{
						printf("Data %i : 0x%X\n" , (i-diff),data[i]);
					}
					break;
				default: 
					
					if (addr64){	// 4 DW
						diff = 16;
					}else{
						diff = 12;
					}
					printf("Data %i : 0x%X\n" , (i-diff),data[i]);
					break;
			}
		}

	}
}

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

