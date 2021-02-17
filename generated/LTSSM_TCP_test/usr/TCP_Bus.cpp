#include "Console_Defines.h"
#include <cstdlib>
#include "TCP_Bus.h"

void TCP_Bus::Connect(uint8_t requester, uint32_t destIP, uint64_t destMAC, uint16_t Port){
 /*    input  [47:0] io_EthernetBus_rx_info_srcMAC,
  input  [47:0] io_EthernetBus_rx_info_destMAC,
  input  [15:0] io_EthernetBus_rx_info_etype,
  input         io_EthernetBus_rx_strb,
  input  [7:0]  io_EthernetBus_rx_data,
  input         io_EthernetBus_rx_empty,
  input         io_EthernetBus_rx_last,
  input         io_EthernetBus_rx_error*/

    uint8_t ip_overhead = 20; // 20 Bytes for IP4 Header
    uint8_t syn_frame[] = {
        0x45, 0x00, // IPV, IHL, DSCP
        0x00, 0x34, // Total Length
        0x00, 0x01, // Ident
        0x40, 0x00, // Flags: Don't fragment
        0x80, 0x06, // TTL 128, TCP
        0x00, 0x00, // Header Checksum -> Calculation method?
        0x00, 0x00, 0x00, 0x00, // Source IP
        0x00, 0x00, 0x00, 0x00, // Dest IP

        // TCP Frame
        0x3A, 0x98, // Src Port
        0x3A, 0x98, // Dest Port
        0x6D, 0x1A, 0x56, 0x37, // Start Sequence Number
        0x00, 0x00, 0x00, 0x00, // Acknowledge Number
        0x50,                   // Header Length in 4Bytes, Flags
        0x02,                   // CWR
        0x20, 0x00,             // Window Size
        0x00, 0x00,             // TCP Checksum
        0x00, 0x00             // Urgent Pointer
    };
    // Save Package Length
    syn_frame[3] = sizeof(syn_frame) & 0xFF;   
    syn_frame[2] = ((sizeof(syn_frame) & 0xFF00)>>8);  

    // Set MAC Adresses
    m_core[requester]->m_core->io_EthernetBus_rx_info_destMAC = m_core[requester]->m_core->io_Params_MAC;
    m_core[requester]->m_core->io_EthernetBus_rx_info_srcMAC = destMAC;
    m_core[requester]->m_core->io_EthernetBus_rx_info_etype = 0x0800;

    // Write IP Addresses
    uint32_t ip_mask = 0xFF;
    for(uint8_t i = 0; i<4;i++){
        // Dest IP
        syn_frame[19-i] = (m_core[requester]->m_core->io_Params_IP&ip_mask)>>(8*i);
        // Source IP
        syn_frame[15-i] = (destIP&ip_mask)>>(8*i);

        ip_mask <<= 8;
    }

    uint16_t tcp_checksum = CalcTCPChecksum(destIP, m_core[requester]->m_core->io_Params_IP, ((uint16_t) (sizeof(syn_frame)-ip_overhead)), &syn_frame[20]);
    //printf("TCP Checksum = 0x%X \n", tcp_checksum);
    syn_frame[37] = tcp_checksum&0xFF;
    syn_frame[36] = ((tcp_checksum&0xFF00)>>8);

    uint16_t ip_checksum = CalcIPChecksum(ip_overhead, syn_frame);
    //printf("IP Checksum = 0x%X \n", ip_checksum);
    syn_frame[11] = ip_checksum&0xFF;
    syn_frame[10] = ((ip_checksum&0xFF00)>>8);

    /*for(uint32_t i = 0; i< sizeof(syn_frame); i++){
        printf("Frame Byte %i   : 0x%X \n", i ,syn_frame[i]);
    }*/
    m_core[requester]->m_core->io_EthernetBus_rx_strb = 1;
    m_core[requester]->m_core->io_EthernetBus_rx_empty = 1;
    CrossPCIe();
    m_core[1]->tick();
    m_core[0]->tick();

    for(uint32_t i = 0; i<sizeof(syn_frame); i++ ){
        m_core[requester]->m_core->io_EthernetBus_rx_strb = 1;
        m_core[requester]->m_core->io_EthernetBus_rx_empty = 0;
        m_core[requester]->m_core->io_EthernetBus_rx_data = syn_frame[i];
        if(i==(sizeof(syn_frame)-1)){
            m_core[requester]->m_core->io_EthernetBus_rx_last = 1;
        }
        CrossPCIe();
            m_core[1]->tick();
            m_core[0]->tick();
        if(i==20){
            CrossPCIe();
            m_core[1]->tick();
            m_core[0]->tick();
            
        }
    }

    m_core[requester]->m_core->io_EthernetBus_rx_last = 0;
    m_core[requester]->m_core->io_EthernetBus_rx_strb = 0;
    CrossPCIe();
    m_core[1]->tick();
    m_core[0]->tick();

}

void TCP_Bus::Send_Data(uint8_t requester, uint32_t destIP, uint64_t destMAC, uint16_t Port, uint32_t ack_number, uint32_t seq_number, uint8_t ack, uint8_t* data, uint16_t Length){

    uint8_t ip_overhead = 20; // 20 Bytes for IP4 Header
    uint8_t syn_frame[] = {
        0x45, 0x00, // IPV, IHL, DSCP
        0x00, 0x28, // Total Length
        0x00, 0x01, // Ident
        0x40, 0x00, // Flags: Don't fragment
        0x80, 0x06, // TTL 128, TCP
        0x00, 0x00, // Header Checksum -> Calculation method?
        0x00, 0x00, 0x00, 0x00, // Source IP
        0x00, 0x00, 0x00, 0x00, // Dest IP

        // TCP Frame
        0x3A, 0x98, // Src Port
        0x3A, 0x98, // Dest Port
        0x6D, 0x1A, 0x56, 0x37, // Start Sequence Number
        0x00, 0x00, 0x00, 0x00, // Acknowledge Number
        0x50,                   // Header Length in 4Bytes
        0x00,                   // CWR, Flags
        0x20, 0x00,             // Window Size
        0x00, 0x00,             // TCP Checksum
        0x00, 0x00             // Urgent Pointer
    };
    
    uint16_t total_length = sizeof(syn_frame)+Length;
    // Save Package Length
    syn_frame[3] = (total_length) & 0xFF;   
    syn_frame[2] = (( total_length & 0xFF00)>>8);  

    // Set MAC Adresses
    m_core[requester]->m_core->io_EthernetBus_rx_info_destMAC = m_core[requester]->m_core->io_Params_MAC;
    m_core[requester]->m_core->io_EthernetBus_rx_info_srcMAC = destMAC;
    m_core[requester]->m_core->io_EthernetBus_rx_info_etype = 0x0800;

    // Write IP Addresses
    uint32_t ip_mask = 0xFF;
    for(uint8_t i = 0; i<4;i++){
        // Dest IP
        syn_frame[19-i] = (m_core[requester]->m_core->io_Params_IP&ip_mask)>>(8*i);
        // Source IP
        syn_frame[15-i] = (destIP&ip_mask)>>(8*i);
        // Ack Number
        syn_frame[31-i] = (ack_number&ip_mask)>>(8*i);
        // Seq Number
        syn_frame[27-i] = (seq_number&ip_mask)>>(8*i);

        ip_mask <<= 8;
    }

    // Set ACK Flag if enabled
    if(ack >= 1){
        syn_frame[33] = 1<<4;
    }

    uint8_t data_frame[total_length];
    for(uint32_t i= 0; i<sizeof(syn_frame); i++){
        data_frame[i] = syn_frame[i];
    }
    for(uint32_t i= 0; i<Length; i++){
        data_frame[sizeof(syn_frame)+i] = data[i];
    }

    uint16_t tcp_checksum = CalcTCPChecksum(destIP, m_core[requester]->m_core->io_Params_IP, ((uint16_t) (sizeof(data_frame)-ip_overhead)), &data_frame[20]);
    //printf("TCP Checksum = 0x%X \n", tcp_checksum);
    data_frame[37] = tcp_checksum&0xFF;
    data_frame[36] = ((tcp_checksum&0xFF00)>>8);

    uint16_t ip_checksum = CalcIPChecksum(ip_overhead, data_frame);
    //printf("IP Checksum = 0x%X \n", ip_checksum);
    data_frame[11] = ip_checksum&0xFF;
    data_frame[10] = ((ip_checksum&0xFF00)>>8);


    




    m_core[requester]->m_core->io_EthernetBus_rx_strb = 1;
    m_core[requester]->m_core->io_EthernetBus_rx_empty = 1;
    CrossPCIe();
    m_core[1]->tick();
    m_core[0]->tick();

    for(uint32_t i = 0; i<total_length; i++ ){
        m_core[requester]->m_core->io_EthernetBus_rx_strb = 1;
        m_core[requester]->m_core->io_EthernetBus_rx_empty = 0;
        m_core[requester]->m_core->io_EthernetBus_rx_data = data_frame[i];
        if(i==(total_length-1)){
            m_core[requester]->m_core->io_EthernetBus_rx_last = 1;
        }
        CrossPCIe();
            m_core[1]->tick();
            m_core[0]->tick();
        if(i==20){
            m_core[1]->tick();
            m_core[0]->tick();
            
        }
    }

    m_core[requester]->m_core->io_EthernetBus_rx_last = 0;
    m_core[requester]->m_core->io_EthernetBus_rx_strb = 0;
    CrossPCIe();
    m_core[1]->tick();
    m_core[0]->tick();

}

void TCP_Bus::Send_TLP(uint8_t requester, uint8_t* data, uint8_t* charisk, uint16_t Length){
    uint8_t x = 0;
    uint8_t receiver = (~requester)&0x01;
    for(uint16_t i = 0; i<Length; i++){

        if(x== 0){
            x = ~x;
            m_core[requester]->m_core->io_GTP_data_rx_charisk  = charisk[i];
            m_core[requester]->m_core->io_GTP_data_rx_data  = data[i];
        }else{
            m_core[receiver]->m_core->io_GTP_data_rx_charisk  = m_core[requester]->m_core->io_GTP_data_tx_charisk;
            m_core[receiver]->m_core->io_GTP_data_rx_data  = m_core[requester]->m_core->io_GTP_data_tx_data;
            m_core[requester]->m_core->io_GTP_data_rx_charisk  |= charisk[i]<<1;
            m_core[requester]->m_core->io_GTP_data_rx_data  |= data[i] << 8;

            m_core[1]->tick();
            m_core[0]->tick();
            x = ~x;
        }
    }
    if(x != 0){
        m_core[receiver]->m_core->io_GTP_data_rx_charisk  = m_core[requester]->m_core->io_GTP_data_tx_charisk;
        m_core[receiver]->m_core->io_GTP_data_rx_data  = m_core[requester]->m_core->io_GTP_data_tx_data;
        m_core[1]->tick();
        m_core[0]->tick();
    }
}

uint32_t TCP_Bus::CalculateTlpCRC(uint8_t* data, uint8_t len)
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

void TCP_Bus::Send_TLP_Checksum(uint8_t requester, uint8_t* data, uint16_t Length){
    uint8_t x = 0;
    uint8_t receiver = (~requester)&0x01;
    uint16_t max_len = Length+2+4;
    uint8_t data_new[max_len];
    uint8_t isk[max_len] = {0};

    data_new[0] = 0xFB;
    isk[0] = 0x1;
    data_new[max_len-1] = 0xFD;
    isk[max_len-1] = 0x1;


    uint32_t checksum = CalculateTlpCRC(data,Length);
    uint32_t mask = 0xFF;
    for(uint8_t i = 0; i<4; i++){
        data_new[max_len-2-i] = (checksum&mask)>>(i*8);
        mask <<= 8;
    }
    for(uint16_t i = 1; i<=Length; i++){
        data_new[i] = data[i-1];
    }
    Send_TLP(requester, data_new, isk, max_len);

}

void TCP_Bus::CrossPCIe(){
    m_core[1]->m_core->io_GTP_data_rx_charisk  = m_core[0]->m_core->io_GTP_data_tx_charisk;
    m_core[1]->m_core->io_GTP_data_rx_data  = m_core[0]->m_core->io_GTP_data_tx_data;
    m_core[0]->m_core->io_GTP_data_rx_charisk  = m_core[1]->m_core->io_GTP_data_tx_charisk;
    m_core[0]->m_core->io_GTP_data_rx_data  = m_core[1]->m_core->io_GTP_data_tx_data;
}

uint16_t TCP_Bus::CalcTCPChecksum(uint32_t destIP, uint32_t srcIP, uint16_t Length, uint8_t* data){
    uint32_t checksum = 0;

    uint32_t ip_mask = 0xFFFF;
    for(uint8_t i = 0; i<2; i++){
        uint16_t temp[2] = {0,0};
        temp[0] = (destIP&ip_mask)>>(i*16);
        temp[1] = (srcIP&ip_mask)>>(i*16);
        checksum += temp[0];
        checksum += temp[1];
        ip_mask <<= 16;
        //printf("0x%X \n",temp[0] );
    }

    checksum += 6; // TCP
    checksum += Length; // TCP


    uint16_t temp = 1;
    for(uint16_t i=0; i<Length; i++){
        checksum += data[i]<<(temp*8);
        if (temp == 1){
            temp = 0;
        }else{
            temp = 1;
        }
    }

    while((checksum & 0xFFFF0000) != 0){
        checksum = ((checksum & 0xFFFF0000)>>16) + (checksum & 0xFFFF);
    }
    return (uint16_t)((~checksum) & 0xFFFF);
}
uint16_t TCP_Bus::CalcIPChecksum(uint16_t Length, uint8_t* data){
    uint32_t checksum = 0;

    uint16_t temp = 1;
    for(uint16_t i=0; i<Length; i++){
        checksum += data[i]<<(temp*8);
        if (temp == 1){
            temp = 0;
        }else{
            temp = 1;
        }
    }
    //printf("IP Checksum = 0x%X \n", checksum);
    while((checksum & 0xFFFF0000) != 0){
        checksum = ((checksum & 0xFFFF0000)>>16) + (checksum & 0xFFFF);
    }
    return (uint16_t)((~checksum) & 0xFFFF);
}

/*
    signal rxsyn_frame: synframe := (
        X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"d",
        X"b", X"d", X"d", X"d", X"f", X"4", X"6", X"6", X"e", X"8", X"c", X"d", X"8", X"d", 
        X"0", X"5", X"6", X"e", X"8", X"4", X"4", X"a", X"a", X"3", X"8", X"0", X"0", X"0", 
        X"5", X"4", X"0", X"0", X"0", X"0", X"4", X"3", X"3", X"3", X"e", X"d", X"0", X"4", 
        X"0", X"0", X"0", X"8", X"6", X"0", X"0", X"4", X"c", X"b", X"0", X"c", X"8", X"a", 
        X"2", X"0", X"b", X"6", X"0", X"c", X"8", X"a", X"2", X"0", X"e", X"6", X"5", X"d", 
        X"5", X"4", X"a", X"3", X"8", X"9", X"d", X"6", X"a", X"1", X"6", X"5", X"7", X"3", 
        X"0", X"0", X"0", X"0", X"0", X"0", X"0", X"0", X"0", X"8", X"2", X"0", X"0", X"2", 
        X"0", X"0", X"5", X"f", X"c", X"b", X"0", X"0", X"0", X"0", X"2", X"0", X"4", X"0", 
        X"5", X"0", X"4", X"b", X"1", X"0", X"3", X"0", X"3", X"0", X"2", X"0", X"1", X"0", 
        X"1", X"0", X"4", X"0", X"2", X"0",
        
        X"7", X"E", X"B", X"0", X"4", X"6", X"3", X"B"       -- CRC  
         );  
            
    type synackframe is array (0 to 131) of std_logic_vector(3 downto 0);         
    signal rxsynack_frame: synackframe := (
        X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"5", X"d",
        X"b", X"d", X"d", X"d", X"f", X"4", X"6", X"6", X"e", X"8", X"c", X"d", X"8", X"d", 
        X"0", X"5", X"6", X"e", X"8", X"4", X"4", X"a", X"a", X"3", X"8", X"0", X"0", X"0", 
        X"5", X"4", X"0", X"0", X"0", X"0", X"8", X"2", X"3", X"3", X"f", X"d", X"0", X"4", 
        X"0", X"0", X"0", X"8", X"6", X"0", X"0", X"4", X"7", X"c", X"0", X"c", X"8", X"a", 
        X"2", X"0", X"b", X"6", X"0", X"c", X"8", X"a", X"2", X"0", X"e", X"6", X"5", X"d", 
        X"5", X"4", X"a", X"3", X"8", X"9", X"d", X"6", X"a", X"1", X"6", X"5", X"8", X"3", 
        X"0", X"0", X"0", X"0", X"0", X"0", X"1", X"0", X"0", X"5", X"0", X"1", X"f", X"f", 
        X"0", X"7", X"7", X"5", X"8", X"0", X"0", X"0", X"0", X"0",
    
        X"4", X"5", X"8", X"D", X"F", X"2", X"7", X"6"       -- CRC  
    );         

*/

void TCP_Bus::Prepare_Address(uint8_t requester, uint32_t IP, uint64_t MAC){

    //  input  [47:0] io_Params_MAC,
    //input  [31:0] io_Params_IP,
    m_core[requester]->m_core->io_Params_IP = IP;
    m_core[requester]->m_core->io_Params_MAC = MAC;
}
