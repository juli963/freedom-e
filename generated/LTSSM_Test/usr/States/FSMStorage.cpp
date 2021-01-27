#include "FSMStorage.h"

void FSMStorage::send_idle(uint8_t n){
    for(uint8_t i=0; i<n; i++){
        m_core->write_RX(0, 0); 
    }
}

void FSMStorage::send_rand(uint8_t n){
    for(uint8_t i=0; i<n; i++){
        m_core->write_RX(std::rand() % 65535, 0); 
    }
}

void FSMStorage::send_skp(uint8_t n, uint8_t pos){   
    if (n == 2){
        m_core->write_RX(0x1C1C, 3); 
    }else{
        uint16_t data = (0x1C<<pos*8) | ((std::rand() % 256) << (1-pos)*8);
        m_core->write_RX(data, (1<<pos)); 
    }
        
}

void FSMStorage::send_ts1(){
    const uint8_t training[5] = {0xF7,0xF7,0x7F,0x02,0x00};

    m_core->write_RX(0xBC | (training[0]<<8) , 3); 
    m_core->write_RX(training[1] | (training[2]<<8) , 1); 
    m_core->write_RX(training[3] | (training[4]<<8) , 0); 
    for(uint8_t i=0;i<5;i++){
        m_core->write_RX(0x4A | (0x4A<<8) , 0); 
    }
}

void FSMStorage::send_ts1(struct PCIe_Types::TrainSet link_data, uint8_t w_skp){
    uint8_t* ptr = (uint8_t*) &link_data;
    if(w_skp){
        m_core->write_RX(0x1C | (0xBC<<8) , 3); 
        m_core->write_RX(ptr[0] | (ptr[1]<<8) , 0); 
        m_core->write_RX(ptr[2] | (ptr[3]<<8) , 0); 
        m_core->write_RX(ptr[4] | (0x4A<<8) , 0); 
        for(uint8_t i=0;i<4;i++){
            m_core->write_RX(0x4A | (0x4A<<8) , 0); 
        }
        m_core->write_RX(0x4A | (0x1C<<8) , 2); 
    }else{
        m_core->write_RX(0xBC | (ptr[0]<<8) , 1); 
        m_core->write_RX(ptr[1] | (ptr[2]<<8) , 0); 
        m_core->write_RX(ptr[3] | (ptr[4]<<8) , 0); 
        for(uint8_t i=0;i<5;i++){
            m_core->write_RX(0x4A | (0x4A<<8) , 0); 
        }
    }
}

void FSMStorage::send_ts2(){
    const uint8_t training[5] = {0xF7,0xF7,0x1F,0x02,0x00};
    
    m_core->write_RX(0xBC | (training[0]<<8) , 3); 
    m_core->write_RX(training[1] | (training[2]<<8) , 1); 
    m_core->write_RX(training[3] | (training[4]<<8) , 0); 
    for(uint8_t i=0;i<5;i++){
        m_core->write_RX(0x45 | (0x45<<8) , 0); 
    }
}

void FSMStorage::send_ts2(struct PCIe_Types::TrainSet link_data, uint8_t w_skp){
    uint8_t* ptr = (uint8_t*) &link_data;
    if(w_skp){
        m_core->write_RX(0x1C | (0xBC<<8) , 3); 
        m_core->write_RX(ptr[0] | (ptr[1]<<8) , 0); 
        m_core->write_RX(ptr[2] | (ptr[3]<<8) , 0); 
        m_core->write_RX(ptr[4] | (0x45<<8) , 0); 
        for(uint8_t i=0;i<4;i++){
            m_core->write_RX(0x45 | (0x45<<8) , 0); 
        }
        m_core->write_RX(0x45 | (0x1C<<8) , 2); 
    }else{
        m_core->write_RX(0xBC | (ptr[0]<<8) , 1); 
        m_core->write_RX(ptr[1] | (ptr[2]<<8) , 0); 
        m_core->write_RX(ptr[3] | (ptr[4]<<8) , 0); 
        for(uint8_t i=0;i<5;i++){
            m_core->write_RX(0x45 | (0x45<<8) , 0); 
        }
    }
}