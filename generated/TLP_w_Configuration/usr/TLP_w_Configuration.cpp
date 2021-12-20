
#include "TLP_w_Configuration.h"

void TLP_w_Configuration_TB::init(){
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

void TLP_w_Configuration_TB::fill_rx(uint16_t isk, uint16_t data){
    qdata_rx.push(data);
    qstrb_rx.push(isk);
}

void TLP_w_Configuration_TB::Create_Train_Set(){
    //Start
    qdata_rx.push(0xBC);
    qstrb_rx.push(0x01);

    // 1st Byte Lane and Port
    qdata_rx.push(0xF7);
    qstrb_rx.push(0x01);
    // 2nd Byte
    qdata_rx.push(0xF7);
    qstrb_rx.push(0x01);
    // 3rd Byte
    qdata_rx.push(0x1F);
    qstrb_rx.push(0x00);
    // 4th Byte
    qdata_rx.push(0x02);
    qstrb_rx.push(0x00);
    // 5th Byte
    qdata_rx.push(0x00);
    qstrb_rx.push(0x00);

    for(uint8_t i = 0; i<10;i++){
        // TS1 ending
        qdata_rx.push(~0x45);
        qstrb_rx.push(0x00);
    }
}

// Send one 0 for Shifting FIFO by 1
void TLP_w_Configuration_TB::Shift_FIFO(){
    qdata_rx.push(0x00);
    qstrb_rx.push(0x00);
}

void TLP_w_Configuration_TB::Create_Configuration_TLP(bool is_read ,bool TD, bool EP, uint16_t Requester, uint8_t Tag, uint16_t Completer, uint16_t Register, uint8_t first_DW_BE, uint32_t data = 0){
    uint8_t len = 0;
    if(is_read){
        len = 14;
    }else{
        len = 18;
    }
    uint8_t arr[len];
    
    /* Start TLP */
    fill_rx(1, 0xFB );
    /* Sequence Number */
    fill_rx(0, ((seq_number&0xFF00)>>8) );
    fill_rx(0, (seq_number&0xFF) );
    arr[0] = ((seq_number&0xFF00)>>8);
    arr[1] = (seq_number&0xFF);

    printf("\n");
    printf("Configuration Request:\n");
    printf("Seq Number: %i\n", seq_number);
    seq_number++;
    
    printf("0x00: ");
    /* Start of Config Request Frame */
        // Type and Format
        if(is_read){
            fill_rx(0, 0x04);  //Cfg Read 0
            printf("0x04 ");
            arr[2] = 0x04;
        }else{
            fill_rx(0, 0x44);  //Cfg Write 0
            printf("0x44 ");
            arr[2] = 0x44;
        }
        //TC, TH etc.
        fill_rx(0, 0x00);
        printf("0x00 ");  
        arr[3] = 0x00;
        //TD, EP etc.
        fill_rx(0, ((TD&0x01)<<7) | ((EP&0x01)<<6)); 
        printf("0x%02X ", ((TD&0x01)<<7) | ((EP&0x01)<<6)) ;  
        arr[4] = ((TD&0x01)<<7) | ((EP&0x01)<<6);
        // Length 
        fill_rx(0, 1);  
        printf("0x01 \n");  
        arr[5] = 1;
        // Requester
        
        printf("0x04: ");  
        fill_rx(0, (Requester&0xFF00)>>8);  
        printf("0x%02X ", (Requester&0xFF00)>>8) ;  
        fill_rx(0, Requester&0xFF);  
        printf("0x%02X ", Requester&0xFF) ;  

        arr[6] = (Requester&0xFF00)>>8;
        arr[7] = Requester&0xFF;
        // Tag 
        fill_rx(0, Tag);  
        printf("0x%02X ", Tag) ;  
        arr[8] = Tag;
        // First DW BE
        fill_rx(0, (first_DW_BE&0x0F) );  
        printf("0x%02X \n", (first_DW_BE&0x0F)) ;  
        arr[9] = (first_DW_BE&0x0F);
        // Completer
        fill_rx(0, (Completer&0xFF00)>>8);  
        printf("0x08: ") ; 
        printf("0x%02X ", (Completer&0xFF00)>>8) ;   
        fill_rx(0, Completer&0xFF);    
        printf("0x%02X ", Completer&0xFF) ; 
        arr[10] = (Completer&0xFF00)>>8;
        arr[11] = Completer&0xFF;
        // Register Number
        fill_rx(0, (Register&0x3C0)>>6);  
        printf("0x%02X ", (Register&0x3C0)>>6) ;   
        fill_rx(0, (Register&0x3F)<<2);  
        printf("0x%02X \n", (Register&0x3F)<<2) ;  
        arr[12] = (Register&0x3C0)>>6;
        arr[13] = (Register&0x3F)<<2;

        if(!is_read){
            fill_rx(0, data&0xFF);  
            printf("0x0C: ") ;  
            printf("0x%02X ", data&0xFF) ;  
            fill_rx(0, (data&0xFF00)>>8);  
            printf("0x%02X ", (data&0xFF00)>>8) ;  
            fill_rx(0, (data&0xFF0000)>>16);  
            printf("0x%02X ", (data&0xFF0000)>>16) ;  
            fill_rx(0, (data&0xFF000000)>>24);  
            printf("0x%02X \n", (data&0xFF000000)>>24) ;  
            arr[14] = data&0xFF;
            arr[15] = (data&0xFF00)>>8;
            arr[16] = (data&0xFF0000)>>16;
            arr[17] = (data&0xFF000000)>>24;
        }

    /* Checksum Creation */
    uint32_t Checksum = CalculateTlpCRC(arr, len);
    printf("Checksum: 0x%08X \n", Checksum) ;  
    fill_rx(0, (Checksum&0xFF000000)>>24 ); 
    fill_rx(0, (Checksum&0xFF0000)>>16 );  
    fill_rx(0, (Checksum&0xFF00)>>8 );  
    fill_rx(0, Checksum&0xFF);  

    /* End TLP */
    fill_rx(1, 0xFD );

}

void TLP_w_Configuration_TB::Create_Custom_TLP(uint8_t* data, uint16_t len){
    
    uint8_t arr[len+2];
    fill_rx(1, 0xFB );

    /* Sequence Number */
    fill_rx(0, ((seq_number&0xFF00)>>8) );
    fill_rx(0, (seq_number&0xFF) );
    arr[0] = ((seq_number&0xFF00)>>8);
    arr[1] = (seq_number&0xFF);
    seq_number++;

    for(uint16_t i = 0; i<len; i++){
        arr[i+2] = data[i];
        fill_rx(0, data[i]);
    }

    /* Checksum Creation */
    uint32_t Checksum = CalculateTlpCRC(arr, len+2);
    printf("Checksum: 0x%08X \n", Checksum) ;  
    fill_rx(0, (Checksum&0xFF000000)>>24 ); 
    fill_rx(0, (Checksum&0xFF0000)>>16 );  
    fill_rx(0, (Checksum&0xFF00)>>8 );  
    fill_rx(0, Checksum&0xFF);  

    /* End TLP */
    fill_rx(1, 0xFD );
}

void TLP_w_Configuration_TB::queue(TLP_w_Configuration_TB* otb){

    /*END or EDB is End*/
    if(in_frame){
        if( ((m_core->io_GTP_data_tx_charisk&0x01) > 0) && ( ((m_core->io_GTP_data_tx_data&0xFF) == 0xFD) || ((m_core->io_GTP_data_tx_data&0xFF) == 0xFE)) ){
            // First Byte
            qdata.push( 0 );
            qstrb.push( 0 );
            in_frame = false;
        }else if( ((m_core->io_GTP_data_tx_charisk&0x02) >0 ) && ( ((m_core->io_GTP_data_tx_data&0xFF00) == 0xFD00) || ((m_core->io_GTP_data_tx_data&0xFF00) == 0xFE00) ) ){
            // Second Byte
            qdata.push( (m_core->io_GTP_data_tx_data&0xFF)  );
            qstrb.push( 1 );

            qdata.push( 0 );
            qstrb.push( 0 );
            in_frame = false;
        }
    }
    if(in_frame){
        qdata.push( (m_core->io_GTP_data_tx_data&0xFF)  );
        qstrb.push( 1 );
        qdata.push( ((m_core->io_GTP_data_tx_data&0xFF00)>>8)  ) ;
        qstrb.push( 1 );
    }

    /*STP trigger*/ 
    if( ((m_core->io_GTP_data_tx_charisk&0x01) > 0) && ((m_core->io_GTP_data_tx_data&0xFF) == 0xFB) ){
        // First Byte
        qdata.push( ((m_core->io_GTP_data_tx_data&0xFF00)>>8)  ) ;
        qstrb.push( 1 );
        in_frame = true;
    }else if( ((m_core->io_GTP_data_tx_charisk&0x02) > 0) && ((m_core->io_GTP_data_tx_data&0xFF00) == 0xFB00) ){
        // Second Byte
        in_frame = true;
    }
    

    Print_Completion_TLP();

    if(!qdata_rx.empty()){
        m_core->io_GTP_data_rx_charisk = qstrb_rx.front()&0x01;
        m_core->io_GTP_data_rx_data = qdata_rx.front();

        qstrb_rx.pop();
        qdata_rx.pop();
        
        if(!qdata_rx.empty()){
            m_core->io_GTP_data_rx_charisk |= (qstrb_rx.front()&0x01)<<1;
            m_core->io_GTP_data_rx_data |= (qdata_rx.front()&0xFF)<<8;

            qstrb_rx.pop();
            qdata_rx.pop();
        }
    }else{
        m_core->io_GTP_data_rx_charisk = 0;
        m_core->io_GTP_data_rx_data = 0;
    }
    
    tick();
}

void TLP_w_Configuration_TB::Print_Completion_TLP(){
    if(!qstrb.empty()){
        if(qstrb.back() == 0){
            
            uint16_t temp16 = (uint16_t)qdata.front()<<8;
            qdata.pop();
            qstrb.pop();
            if(!qdata.empty()){
                temp16 |= qdata.front();

                qdata.pop();
                qstrb.pop();
            }

            printf("\n");
            printf("Configuration Answer:\n");
            printf("Seq Number: %i\n", temp16);
            Create_ACK_DLLP(true, temp16);

            uint16_t i = 0;
            while(!qstrb.empty() && !qstrb.front() == 0){   
                printf("0x%02X: ", i);

                for(uint8_t x = 0; x<4;x++){
                    if(!qdata.empty()){
                        if(!qstrb.front() == 0){
                            printf("0x%02X ", qdata.front());
                        }
                        qdata.pop();
                        qstrb.pop();
                    }
                }
                printf("\n");
                i = i+4;
            }
            while(!qstrb.empty()){
                qdata.pop();
                qstrb.pop();
            }
        }
    }
}

void TLP_w_Configuration_TB::Create_ACK_DLLP(bool is_ACK, uint16_t seq_number){
    uint8_t arr[4];
    fill_rx(1, 0x5C);
    if(is_ACK){
        fill_rx(0, 0); 
        arr[0] = 0; 
    }else{
        fill_rx(0, 0x10);  
        arr[0] = 0x10; 
    }
    
    fill_rx(0, 0x00);
    arr[1] = 0x00; 
    fill_rx(0, (seq_number&0x0F00)>>8);  
    fill_rx(0, seq_number&0x00FF);  
    arr[2] = (seq_number&0x0F00)>>8; 
    arr[3] = seq_number&0x00FF; 

    uint16_t Checksum = CalculateDllpCRC(arr[0], &arr[1], 4);
    fill_rx(0, (Checksum&0xFF00)>>8);  
    fill_rx(0, Checksum&0x00FF);  


    fill_rx(1, 0xFD); 
}

uint32_t TLP_w_Configuration_TB::CalculateTlpCRC(uint8_t* data, uint8_t len)
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

uint16_t TLP_w_Configuration_TB::CalculateDllpCRC(uint8_t type, uint8_t* data, uint8_t len)
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




void TLP_w_Configuration_TB::decode_type(uint8_t format, uint8_t type){
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
uint8_t TLP_w_Configuration_TB::tlp_data_format(uint8_t format, uint8_t type){

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

void TLP_w_Configuration_TB::decode_Message_Code(uint8_t code, uint8_t r){
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

void TLP_w_Configuration_TB::decode_TLP(uint8_t* data, uint8_t len){
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