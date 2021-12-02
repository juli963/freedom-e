
#include "ESP_FIFO_Test.h"

void ESP_FIFO_Test_TB::init(){
    printf("ESP FIFO Testbench Initialized\n");
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}

void ESP_FIFO_Test_TB::add_rx_fifo(){
    if(m_core->io_deq_Data_new >0 ){
        ESP_data temp;
        temp.type = m_core->io_recv_type;
        temp.ident = m_core->io_recv_Ident;
        temp.cmd = m_core->io_recv_cmd;
        temp.address = m_core->io_recv_address;
        temp.data = m_core->io_recv_Data;

        esp_rx_fifo.push(temp);
    }
}

void ESP_FIFO_Test_TB::send_back(){
    if(!esp_rx_fifo.empty()){
        m_core->io_enq_Data = 1;

        m_core->io_trans_Data = esp_rx_fifo.front().data;
        m_core->io_trans_type = esp_rx_fifo.front().type;
        m_core->io_trans_cmd = esp_rx_fifo.front().cmd;
        m_core->io_trans_address = esp_rx_fifo.front().address;
        m_core->io_trans_Ident = esp_rx_fifo.front().ident;
    }
    
}

void ESP_FIFO_Test_TB::check_rx_fifo_tick(){
    if(!esp_rx_fifo.empty()){
        if( esp_rx_fifo.front().data == rx_start_data ){
            printf("Received Data OK Data: 0x%08X\n", rx_start_data);
            
        }else{
            printf("Received Data FAIL Data: 0x%08X\n", rx_start_data);
        }
        rx_start_data++;
        esp_rx_fifo.pop();
    }
}

void ESP_FIFO_Test_TB::add_tx_fifo(uint8_t type, uint8_t ident, uint8_t cmd, uint8_t address, uint32_t data){
    ESP_data temp;
    temp.type = type;
    temp.ident = ident;
    temp.cmd = cmd;
    temp.address = address;
    temp.data = data;

    esp_tx_fifo.push(temp);
}

void ESP_FIFO_Test_TB::check_tx_fifo_tick(){
    if(!esp_tx_fifo.empty()){
        if( esp_tx_fifo.front().data == tx_start_data ){
            printf("Transmitted Data OK Data: 0x%08X\n", tx_start_data);
            
        }else{
            printf("Transmitted Data FAIL Data: 0x%08X\n", tx_start_data);
        }
        tx_start_data++;
        esp_tx_fifo.pop();
    }
}