
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
        fill_rx(0, (Register&0x3C0)>>5);  
        printf("0x%02X ", (Register&0x3C0)>>5) ;   
        fill_rx(0, (Register&0x3F)<<2);  
        printf("0x%02X \n", (Register&0x3F)<<2) ;  
        arr[12] = (Register&0x3C0)>>5;
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
