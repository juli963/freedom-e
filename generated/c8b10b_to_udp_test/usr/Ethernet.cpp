
#include "Ethernet.h"

//#define DEBUG // Couple TX Back to RX


void Ethernet::init(){
    printf("Ethernet Module included\n");
}

void Ethernet::tick(){  
   // Transmitter
    if(!tx_length.empty() && tx_act_length == 0 && tx_gap == 0){
        tx_gap = 10;
        tx_act_length = tx_length.front();
        tx_length.pop();
        tb->m_core->io_EthernetBus_rx_empty = 0;

        tb->m_core->io_EthernetBus_rx_info_destMAC = 0;
        tb->m_core->io_EthernetBus_rx_info_srcMAC = 0;
        tb->m_core->io_EthernetBus_rx_info_etype = 0;

        for(uint8_t i = 0; i<6; i++){
            tb->m_core->io_EthernetBus_rx_info_destMAC |= (uint64_t)tx_data.front() << ((5-i)*8);
            tx_data.pop();
        }
        for(uint8_t i = 0; i<6; i++){
            tb->m_core->io_EthernetBus_rx_info_srcMAC |= (uint64_t)tx_data.front() << ((5-i)*8);
            tx_data.pop();
        }
        for(uint8_t i = 0; i<2; i++){
            tb->m_core->io_EthernetBus_rx_info_etype |= tx_data.front() << ((1-i)*8);
            tx_data.pop();
        }
        tx_act_length -= 14;
        //printf("TX Frame Length: %i \n" ,tx_act_length);
        
    }
    if(tx_act_length > 0){
        tb->m_core->io_EthernetBus_rx_data = tx_data.front();
        tx_data.pop();
        tb->m_core->io_EthernetBus_rx_strb = 1;
        if (tx_act_length == 1){
            tb->m_core->io_EthernetBus_rx_last = 1;
        }
        tx_act_length--;
    }else {
        if(tx_gap > 0){
            tx_gap--;
        }
        
        tb->m_core->io_EthernetBus_rx_last = 0;
        tb->m_core->io_EthernetBus_rx_strb = 0;
    }


    // Receiver
    tb->m_core->io_EthernetBus_tx_ready = 1;
    if(tb->m_core->io_EthernetBus_tx_run > 0){    // In Frame
        if(tb->m_core->io_EthernetBus_tx_strb > 0){
            if(!in_rx){ // Start of Frame
                in_rx = true;
            }
            rx_data.push(tb->m_core->io_EthernetBus_tx_data);
            rx_act_length++;
        }
        if(tb->m_core->io_EthernetBus_tx_strb == 0 && last_strb >0){
            rx_length.push(rx_act_length);
            rx_act_length = 0;
            in_rx = false;
        }
        last_strb = tb->m_core->io_EthernetBus_tx_strb;
    }

}

void Ethernet::fill_tx_fifo(uint8_t* data, uint16_t length){    // Prints Input Datastream
    for(uint16_t i = 0; i<length;i++){
        tx_data.push(data[i]);
    }
    tx_length.push(length);
}  

void Ethernet::clear_rx_fifo(){   // Prints Output Datastream
    if (!rx_length.empty()){
        uint16_t length = rx_length.front();
        rx_length.pop();
        //printf("Length: %i\n", length);
        uint8_t data[length];

        for(uint16_t i=0; i<length;i++){
            data[i] = rx_data.front();
            rx_data.pop();
        }

        // Ethernet Decoding
        printEthernet(data, length);
    }
        
}   

void Ethernet::printEthernet(uint8_t* data, uint16_t length){
    //uint64_t destMAC = data[0] << 40 | data[1] << 32 | data[2] << 24 | data[3] << 16 | data[4] << 8 | data[5];
    //uint64_t srcMAC = data[6] << 40 | data[7] << 32 | data[8] << 24 | data[9] << 16 | data[10] << 8 | data[11];
    uint8_t Src_MAC[6] = {
        data[0], data[1], data[2], data[3], data[4], data[5]
    };
    uint8_t Dest_MAC[6] = {
        data[6], data[7], data[8], data[9], data[10], data[11]
    };

    uint16_t ethType = data[12] << 8 | data[13];

    uint8_t* next_data = &data[14];
    uint16_t next_length = length-14;

    if(print_output){
    printf("------- RX Ethernet Frame ---------\n");
    printf("\t Destination MAC: ");//0x%012lX \n", destMAC);
    for(uint8_t i = 0; i<6;i++){
        
        printf("%02X", Dest_MAC[i]);
        if(i < 5 ){
            printf(":");
        }
    }

    printf("\n");
    printf("\t Source MAC: ");//0x%012lX \n", srcMAC);
    for(uint8_t i = 0; i<6;i++){
        printf("%02X", Src_MAC[i]);
        if(i < 5 ){
            printf(":");
        }
    }
    printf("\n\t EtherType: 0x%04X \n", ethType);
    }
    switch(ethType){
        case 0x0800:    // IP Package
            printIP(next_data, next_length);
            break;
        case 0x0806:    // IP Package
            printARP(next_data, next_length);
            break;
        default: 
            if(print_output){
            printf("\t ------Unimplemented Ethertype-----\n \t Data:");
            for(uint16_t i = 0; i<next_length; i++){
                if(i%8 == 0){
                    printf("\n\t ");
                }
                printf("0x%02X ", next_data[i]);
            }
            printf("\n");
            }
            break;
    }
}

void Ethernet::printIP(uint8_t* data, uint16_t length){
    uint16_t protocol;
    uint8_t* next_data = nullptr;
    uint16_t next_length = 0;
    uint8_t pseudo_header[9];

    if((data[0] & 0xF0) == 0x40){  // IPv4
        uint8_t IHL = (data[0] & 0x0F) ;
        uint8_t TOS = data[1];
        uint16_t FrameLength = data[3] | (data[2] << 8);
        uint16_t Ident = data[5] | (data[4] << 8);
        uint8_t Flags = (data[6] & 0xF8)>>5;
        uint16_t Offset = ((data[6] & 0x07) << 8) | (data[7]);
        uint8_t TTL = data[8];
        protocol = data[9];
        uint16_t Checksum = (data[10] << 8) | data[11];
        uint8_t Src_IP[4] = {
            data[12], data[13], data[14], data[15]
        };
        for(uint8_t i= 0;i<4; i++){
            pseudo_header[i] = Src_IP[i];
        }
        uint8_t Dest_IP[4] = {
            data[16], data[17], data[18], data[19]
        };
        for(uint8_t i= 0;i<4; i++){
            pseudo_header[i+4] = Dest_IP[i];
        }
        pseudo_header[8] = protocol;

        uint16_t Option_Length = (IHL-5)*4;
        
        next_data = &data[IHL*4];
        next_length = length-(IHL*4);

        uint8_t check_data[IHL*4];
        memcpy(check_data, data, IHL*4);
        check_data[10] = 0;
        check_data[11] = 0;
        
        if(print_output){
        printf("\t IPv4\n");
        printf("\t\t IHL: 0x%01X\n", IHL);
        printf("\t\t TOS: 0x%02X\n", TOS);
        printf("\t\t FrameLength: %i\n", FrameLength);
        printf("\t\t Ident: 0x%04X\n", Ident);
        printf("\t\t Flags: 0x%01X\n", Flags);
        printf("\t\t Offset: %i\n", Offset);
        printf("\t\t TTL: %i\n", TTL);
        printf("\t\t Protocol: 0x%04X\n", protocol);
        printf("\t\t Checksum: 0x%04X, Valid:%s \n", Checksum, checkIPChecksum(check_data, IHL*4, Checksum)?"true":"false");

        printf("\t\t Src IP: ");
        for(uint8_t i = 0; i<4;i++){
            printf("%i", Src_IP[i]);
            if(i < 3 ){
                printf(".");
            }
        }
        printf("\n\t\t Dest IP: ");
        for(uint8_t i = 0; i<4;i++){
            printf("%i", Dest_IP[i]);
            if(i < 3 ){
                printf(".");
            }
        }
        printf("\n");

        if(Option_Length > 0 ){
            printf("\t\t Option Data:\n");
            for(uint16_t i = 0; i<Option_Length; i++){
                printf("\t\t\t %X %X %X %X\n", data[20+i], data[21+i], data[22+i], data[23+i] );
            }
        }
        }

    }else{  // IPv6
        if(print_output){
        printf("\t\t IPv6\n");
        }
    }

    switch(protocol){
        case 6: // TCP
            printTCP(next_data, next_length);
            break;
        case 17: // UDP
            printUDP(next_data, next_length, pseudo_header);
            break;
        default: 
            if(print_output){
            printf("\t\t ------Unimplemented IP Type-----\n \t\t Data:");
            for(uint16_t i = 0; i<next_length; i++){
                if(i%8 == 0){
                    printf("\n\t\t ");
                }
                printf("0x%02X ", next_data[i]);
            }
            printf("\n");
            }
            break;
    }
}

void Ethernet::printUDP(uint8_t* data, uint16_t length, uint8_t* pseudo_header = nullptr){
    uint16_t src_Port = (data[0] << 8) | data[1];
    uint16_t dest_Port = (data[2] << 8) | data[3];
    uint16_t frame_length = (data[4] << 8) | data[5];
    uint16_t checksum = (data[6] << 8) | data[7];

    uint8_t* next_data = &data[8];
    
    uint16_t next_length = length-8;
    if(length >= 8){
        uint8_t check_data[length+12];
        memcpy(&check_data[12], data, length);
        for(uint8_t i = 0; i<8; i++){
            check_data[i] = pseudo_header[i];
            //printf("0x%02X ", pseudo_header[i]);
        }
        check_data[8] = 0;
        check_data[9] = pseudo_header[8];
        check_data[10] = data[4];
        check_data[11] = data[5];
        check_data[18] = 0;
        check_data[19] = 0;
        if(print_output){
        printf("\t\t UDP\n");
        printf("\t\t\t Source Port: %i\n", src_Port);
        printf("\t\t\t Destination Port: %i\n", dest_Port);
        printf("\t\t\t Length: %i\n", frame_length);
        printf("\t\t\t Data Length: %i\n", (frame_length-8));
        printf("\t\t\t Checksum: 0x%04X, Valid:%s\n", checksum, checkIPChecksum(check_data, length+12, checksum)?"true":"false");
        }
        switch(dest_Port){
            /*case 15000: // ESP
                printESP(next_data, next_length);
                break;*/
            default: 
                if(print_output){
                    printf("\t\t ------Unimplemented UDP Port-----\n \t\t\t Data:");
                    for(uint16_t i = 0; i<next_length; i++){
                        if(i%8 == 0){
                            printf("\n\t\t\t ");
                        }
                        printf("0x%02X ", next_data[i]);
                    }
                    printf("\n");
                }
                if (next_length > 24){
                    for(uint16_t i = 24; i<next_length; i++){
                        if(i%8 == 0){
                            //printf("\n\t\t ");
                        }
                        //printf("0x%02X ", next_data[i]);
                    }
                    //printf("\n");
                    


                    uint16_t index = 24;
                    s8b10b temp;
                    //printf("\n"/*\t\t\t "*/);
                    uint8_t mask = 0x01;
                    uint8_t shift = 0x00;
                    uint16_t data_len = 0;
                    for(uint16_t i = 0; i<next_length-24; i++){
                        if( shift < 7 ){
                            temp.data = next_data[index];
                            //printf("ISK Index %i\n", ((i/8)*8)+7);
                            if( ((i/8)*8)+7 < next_length-25 ){
                                //printf("Mask 0x%02X ", mask);
                                //printf("ISK 0x%02X ", next_data[((i/8)*8)+7+24]);
                                //printf("Shift 0x%02X ", shift);
                                //printf("ISK 0x%02X \n", next_data[((i/7)*7)+8+24]);
                                // printf("SPECIAL %i %i \n", ((i/7)*7)+shift, next_length-26 );
                                temp.isk = (next_data[((i/8)*8)+7+24] & mask) >> shift;
                                //temp.isk = (next_data[((i/7)*7)+7+24] & mask) << shift;
                            }else{
                                //printf("SPECIAL \n");
                                temp.isk = (next_data[next_length-1] & mask) >> shift;
                                //temp.isk = (next_data[next_length-1] & mask) << shift;
                            }
                            mask <<= 1;
                            shift++;
                            // next line
                            //printf("\n\t\t\t ");
                            if(i < next_length-25){
                                //printf("Data: 0x%02X %i\n", temp.data, temp.isk);
                                fifo_data.push(temp);
                                data_len++;
                            }
                            
                        }else{
                            mask = 0x01;
                            shift = 0;
                            //printf("\n");
                            //printf("\n\t\t\t ");
                        }

                        index++;
                        
                    }
                    fifo_mgmt.push(data_len);
                    //printf("\n");
                }
                break;
        }
    }else{
        if(print_output){
        printf("\t\t Error: UDP Frame too short.\n");
        }
    }
}


void Ethernet::printTCP(uint8_t* data, uint16_t length){


}

void Ethernet::printARP(uint8_t* data, uint16_t length){
    uint16_t HW_Type = (data[0] << 8) | data[1];
    uint16_t Protocol_Type = (data[2] << 8) | data[3];
    uint8_t HW_Size = data[4];
    uint8_t Proto_Size = data[5];
    uint8_t Operation = (data[6] << 8) | data[7];

    uint8_t Src_MAC[6] = {
        data[8], data[9],data[10], data[11], data[12], data[13]
    };
    uint8_t Src_IP[4] = {
        data[14], data[15],data[16], data[17]
    };
    uint8_t Dest_MAC[6] = {
        data[18], data[19],data[20], data[21], data[22], data[23]
    };
    uint8_t Dest_IP[4] = {
        data[24], data[25],data[26], data[27]
    };

    if(print_output){
    printf("\t ARP\n");
    printf("\t\t Hardware Type: 0x%04X\n", HW_Type);
    printf("\t\t Protocol Type: 0x%04X\n", Protocol_Type);
    printf("\t\t Hardware Size: 0x%02X\n", HW_Size);
    printf("\t\t Protocol Size: 0x%02X\n", Proto_Size);
    printf("\t\t Operation: 0x%04X\n", Operation);

    printf("\t\t Src MAC: ");
    for(uint8_t i = 0; i<6;i++){
        printf("%i", Src_MAC[i]);
        if(i < 5 ){
            printf(":");
        }
    }
    printf("\n");

    printf("\t\t Src IP: ");
    for(uint8_t i = 0; i<4;i++){
        printf("%i", Src_IP[i]);
        if(i < 3 ){
            printf(".");
        }
    }
    printf("\n");

    printf("\t\t Dest MAC: ");
    for(uint8_t i = 0; i<6;i++){
        printf("%i", Dest_MAC[i]);
        if(i < 5 ){
            printf(":");
        }
    }
    printf("\n");

    printf("\t\t Dest IP: ");
    for(uint8_t i = 0; i<4;i++){
        printf("%i", Dest_IP[i]);
        if(i < 3 ){
            printf(".");
        }
    }
    printf("\n");
    }
}

void Ethernet::printESP(uint8_t* data, uint16_t length){
    uint8_t type = data[0];
    uint8_t ident = data[1];
    uint8_t cmd = data[2];
    uint8_t address = data[3];
    uint32_t regdata = (data[7]<<24) | (data[6]<<16) | (data[5]<<8) | data[4];
    if(print_output){
    printf("\t\t\t\t ESP FIFO Potocol\n");
    printf("\t\t\t\t\t Type: 0x%02X\n", type);
    printf("\t\t\t\t\t Ident: 0x%02X\n", ident);
    printf("\t\t\t\t\t Command: 0x%02X\n", cmd);
    printf("\t\t\t\t\t Address: 0x%02X\n", address);
    printf("\t\t\t\t\t Data: 0x%08X\n", regdata);
    }
    //tb->add_tx_fifo(type, ident, cmd, address, regdata);
}

bool Ethernet::checkIPChecksum(uint8_t* data, uint16_t length, uint16_t checksum){
    if(checksum == 0){
        return true;
    }else{
        return calcIPChecksum(data,length) == checksum;
    }
}

uint16_t Ethernet::calcIPChecksum(uint8_t* data, uint16_t length){
    uint32_t checksum = 0;
    uint16_t temp = 0;
    for(uint16_t i=0; i<length; i++){
        if(i%2 == 0){
            temp = data[i] << 8;
        }else{
            
            temp = temp | data[i];
            //printf("0x%X ", temp);
            checksum = checksum + temp;
            temp = 0;
        }
    }
    checksum = checksum + temp;
    while((checksum & 0xFFFF0000) > 0){
        checksum = (checksum&0xFFFF) + ((checksum&0xFFFF0000)>>16);
    }
   //printf("\n 0x%X\n", checksum);
    return ~checksum;

}

void Ethernet::createEthernetFrame(uint8_t* destMAC, uint16_t etherType, uint8_t* data, uint16_t length){   // Prints Output Datastream
    
    
    for(uint8_t i = 0; i< 6; i++){
        tx_data.push(destMAC[i]);
    }
    for(uint8_t i = 0; i< 6; i++){
        tx_data.push(srcMAC[i]);
        //printf("0x%X \n", srcMAC[i]);
    }
    tx_data.push((etherType&0xFF00) >> 8);
    tx_data.push(etherType&0x00FF);
    tx_data.push(0x00);

    for(uint8_t i =0; i<length;i++){
        tx_data.push(data[i]);
    } 
    tx_length.push(6+6+2+length+1);

#if defined(DEBUG)
    for(uint8_t i = 0; i< 6; i++){
        rx_data.push(destMAC[i]);
    }
    for(uint8_t i = 0; i< 6; i++){
        rx_data.push(srcMAC[i]);
        //printf("0x%X \n", srcMAC[i]);
    }
    rx_data.push((etherType&0xFF00) >> 8);
    rx_data.push(etherType&0x00FF);
    //tx_data.push(0x00);

    for(uint8_t i =0; i<length;i++){
        rx_data.push(data[i]);
    } 
    rx_length.push(6+6+2+length);
    //printf("Frame Registered, Size %i\n", tx_length.front());
#endif
}   

void Ethernet::createARPFrame(uint8_t* searchIP){ 
printf("------- TX ARP Frame ---------\n\n");
    uint8_t data[28] = {0};
    uint8_t mac[6] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    data[1] = 1;    // HW Type
    data[2] = 8;    // Protocol Type
    data[4] = 6;    // HW Size
    data[5] = 4;    // Protocol Size
    data[7] = 1;    // Operation
    
    for(uint8_t i = 0; i<6; i++){
        data[8+i] = srcMAC[i];
    }
    for(uint8_t i = 0; i<4; i++){
        data[14+i] = srcIP[i];
    }

    for(uint8_t i = 0; i<4; i++){
        data[24+i] = searchIP[i];
    }

    createEthernetFrame(mac, 0x0806, data, 28);
} 

void Ethernet::createIP4Frame(uint8_t* data, uint16_t length, uint8_t* destIP, uint8_t* destMAC, uint8_t protocol){  
//printf("------- TX IP4 Frame ---------\n\n");
    uint8_t next_data[20+length] = {0};
    uint16_t next_length = length+20;

    memcpy(&next_data[20], data, length);
    memcpy(&next_data[16], destIP, 4);
    memcpy(&next_data[12], srcIP, 4);

    next_data[0] = 0x45;   // Version 4 and IHL = 5

    next_data[2] = (next_length&0xFF00)>>8;   // Length Upper
    next_data[3] = next_length&0x00FF;   // Length Lower

    next_data[8] = 100; // TTL
    next_data[9] = protocol;    // Protocol

    uint16_t checksum = calcIPChecksum(next_data, 20);

    next_data[10] = (checksum&0xFF00)>>8;
    next_data[11] = (checksum&0x00FF);

    createEthernetFrame(destMAC, 0x0800, next_data, next_length);
} 

void Ethernet::createUDPFrame(uint8_t* data, uint16_t length, uint8_t* destIP, uint8_t* destMAC, uint16_t srcPort, uint16_t destPort){  
    uint16_t next_length = length+8;
    uint8_t next_data[next_length+12] = {0};

    

    memcpy(&next_data[20], data, length);
    
    // Pseudo Header
    memcpy(&next_data[0], srcIP, 4);
    memcpy(&next_data[4], destIP, 4);
    next_data[9] = 17;
    next_data[10] = ((length+8)&0xFF00)>>8;
    next_data[11] = ((length+8)&0x00FF);

    // UDP Header
    next_data[12] = (destPort&0xFF00)>>8;
    next_data[13] = (destPort&0x00FF);
    next_data[14] = (srcPort&0xFF00)>>8;
    next_data[15] = (srcPort&0x00FF);
    next_data[16] = ((length+8)&0xFF00)>>8;
    next_data[17] = ((length+8)&0x00FF);
    uint16_t checksum = calcIPChecksum(next_data, next_length+12);
    next_data[18] = (checksum&0xFF00)>>8;
    next_data[19] = (checksum&0x00FF);


    createIP4Frame(&next_data[12], next_length, destIP, destMAC, 17);
}   

//Type(8, 0) Length(31, 8 ), Flags(63, 32), Checksum(79, 64), RSVD(127, 80)
/*void Ethernet::createStreamFrame(uint8_t* destIP, uint8_t* destMAC, uint8_t type, uint32_t flags, uint64_t rsvd, uint32_t* data, uint16_t data_len){
    uint16_t length = data_len+11;
    uint8_t arr_data[length] = {
        type, ident, cmd, address,
        0,0,0,0
    };

    for(uint8_t i = 0; i<4; i++){
        arr_data[i+4] = data&0xFF;
        data >>= 8;
    }
    createUDPFrame(arr_data, length, destIP, destMAC, 15000, 15000);
}   */