//#include "../obj_dir/VLTSSM_TCP_test.h"
#include "TCP_Bus.h"
#include "Console_Defines.h"
 #include "LTSSM_TCP_test.h"

uint16_t CalculateDllpCRC(uint8_t type, uint8_t* data, uint8_t len = 4);
uint32_t CalculateTlpCRC(uint8_t* data, uint8_t len);
void decode_TLP(uint8_t* data, uint8_t len);

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
	tcp_stack->Connect(1,0xC0A86401,0x111213141820, 15000);
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
	uint8_t tlp_testdat[]  = {  0x02,0x03,0x04,0x05,0x06 	};

	uint8_t tlp_isk [] = { 	0x01, 0x00 ,0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00,0x01
	};
	uint8_t tlp_new2 [] = { 	 0x04, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x0f, 0x03, 0x00, 0x00, 0x00 };
	uint8_t tlp_new3 [] = { 	 0x04, 0x00, 0x00, 0x01, 0x00, 0xc0, 0x00, 0x0f, 0x03, 0x00, 0x00, 0x00, 0xca, 0x28, 0xa8, 0x44  };
	uint8_t tlp_ans [] = { 	 0x4a, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0xc0, 0x00, 0x00, 0xde, 0x10, 0x22, 0x06  };

 	decode_TLP(&tlp_dat[2], sizeof(tlp_dat)-2);
	printf("Next \n \n ");
 	decode_TLP(tlp_new2, sizeof(tlp_new2));
 	decode_TLP(tlp_new2, sizeof(tlp_new2));
 	decode_TLP(tlp_ans, sizeof(tlp_ans));

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

	//tcp_stack->Send_TLP_Checksum(0, tlp_dat, sizeof(tlp_dat));

	tcp_stack->Send_Data(1, 0xC0A86401,0x111213141820, 15000, 0x0, 0x6D1A5638, 0, tlp_testdat, 5);
	tcp_stack->Send_Data(0, 0xC0A86410,0x111213140901, 15000, 0x15, 0x6D1A5638, 1, tlp_testdat, 5);

	for(uint16_t i = 0; i<30000; i++){
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
void decode_TLP(uint8_t* data, uint8_t len){
	if (len >= 12){
		uint8_t format,type;
		uint16_t Length;
		for(uint8_t i = 0; i<len ; i++)
		{
			switch(i){
				case 0: // FMT and Type
					format = (data[i]&0xE0)>>5;
					type = (data[i]&0x1F);

					decode_type(format,type);
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
					Length = (data[i]&0x03)<<8;
					break;
				case 3: // Length
					Length |= data[i];
					printf("Length: %i DW\n", Length);
					break;
				case 4: //  Vary with type

					break;
				case 5: //  Vary with type

					break;
				case 6: //  Vary with type

					break;
				case 7: // Byte Enables
					switch(((data[i]&0xF0)>>4)){
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
					break;
				case 8: //  Vary with type

					break;
				case 9: //  Vary with type

					break;
				case 10: //  Vary with type

					break;
				case 11: //  Vary with type

					break;
				default: 
					uint8_t diff = 0;
					if ((format & 0x03) == 3){	// 4 DW
						diff = 16;
					}else{
						diff = 12;
					}
					printf("Data %i : 0x%X" , (i-diff),data[i]);
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

