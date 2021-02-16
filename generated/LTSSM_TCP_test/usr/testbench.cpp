//#include "../obj_dir/VLTSSM_TCP_test.h"
#include "TCP_Bus.h"
#include "Console_Defines.h"
 #include "LTSSM_TCP_test.h"

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

	TCP_Bus *tcp_stack = new TCP_Bus(tb,tb2);

	tcp_stack->Prepare_Address(0, 0xC0A86401, 0x111213141820);
	tcp_stack->Prepare_Address(1, 0xC0A86410, 0x111213140901);


	tb->opentrace("trace.vcd");
	//tb2->opentrace("trace.vcd");

	const char *state_names[11] = { "Detect", "Polling", 
                             "Configuration", "Recovery", "L0", "L0s", "L1", "L2", "Disabled", "Loopback", "Hot Reset" };

	enum e_Function{f_device=0, f_root=1};

	printf("Start as Root Port\n");

	tb2->m_core->io_pRootPort = f_device;
	tb->m_core->io_pRootPort = f_root;
	tb->init();
	tb2->init();
	//tb->tick();
	//tb->tick();
	//tb->tick();
	tcp_stack->Connect(0,0xC0A86410,0x111213140901, 15000);
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

	uint8_t tlp_data[]  = { 0xFB, 0x00 ,0x00, 0x74, 0x00,
							0x00, 0x01, 0x00, 0xC0, 0x00, 0x50,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x75, 0x76, 0x56, 0x23,0xFD
	};
	uint8_t tlp_dat[]  = {  0x00 ,0x00, 0x74, 0x00,
							0x00, 0x01, 0x00, 0xC0, 0x00, 0x50,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x71
							
	};

	uint8_t tlp_isk [] = { 	0x01, 0x00 ,0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00,0x01
	};
	tcp_stack->Send_TLP(0, tlp_data, tlp_isk, sizeof(tlp_data));

	for(uint16_t i = 0; i<300; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}

	tcp_stack->Send_TLP_Checksum(0, tlp_dat, sizeof(tlp_dat)-1);

	for(uint16_t i = 0; i<300; i++){
		tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
		tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;

		tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
		tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
		tb->tick();
		tb2->tick();
	}

	tcp_stack->Send_TLP_Checksum(0, tlp_dat, sizeof(tlp_dat));

	for(uint16_t i = 0; i<300; i++){
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

	uint8_t arr[5] = {0x00,0x40,0x40,0x19,0xA4};
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x60,arr));
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x50,arr));
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x40,arr));
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x40,arr,6));
	arr[3] = 0xF2; 
	arr[4] = 0xC3; 
	printf("CRC: 0x%X \n", CalculateDllpCRC(0x50,arr,6));

	//printf("TLPCRC: 0x%X \n", CalculateTlpCRC(tlp_arr,21));
	//printf("TLPCRC: 0x%X \n", CalculateTlpCRC(tlp_arr,22));
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

