
#include "Console_Defines.h"
#include "i2c_to_udp_test.h"
#include "Memory.h"
#include "Ethernet.h"
#include "I2C_Simulator.h"

int main(int argc, char **argv) {

        i2c_to_udp_test_TB *tb = new i2c_to_udp_test_TB();
        Ethernet eth_intf(tb);
        I2C_Simulator i2c_intf(tb);
        Memory mem_intf(tb, 10);
        tb->opentrace("trace.vcd");
        tb->init();
        printf("---------------------------------------------\n");
        tb->init();
        eth_intf.init();
        i2c_intf.init();
        mem_intf.init();
        printf("\n");

        tb->m_core->io_eth_params_MAC = 0xFFAFABACAD;
        tb->m_core->io_eth_params_IP = 0xC0A80206;
        
        //(uint8_t type, uint32_t flags, uint16_t length, uint8_t* data, uint64_t timestamp_start, uint64_t timestamp_end){
        
        /*const uint8_t len = 32;
        uint8_t data[len];
        for(uint8_t i = 0; i<len; i++){
                data[i] = rand() % 255;
                printf("0x%02X ", data[i]);
                if(i%8 == 0){
                        printf("\n");
                }
        }
        printf("\n");

        for(uint8_t i=0; i<9; i++){
                mem_intf.add_data(0x14+i, 0x03040506070809, len-i, data, 0xABCDEF1211100908, 0xCFADFF1011121314);
        }*/


        I2C_Simulator::I2C_Header sHeader;
        sHeader.slave_address = 0b1000001;
        sHeader.rw = 0;
        sHeader.length = 2;
        i2c_intf.fifo_header.push(sHeader);
        i2c_intf.fifo_data.push(0x07);
        i2c_intf.fifo_data.push(0x08);

        sHeader.slave_address = 0b1000011;
        sHeader.rw = 0;
        sHeader.length = 4;
        i2c_intf.fifo_header.push(sHeader);
        i2c_intf.fifo_data.push(0x07);
        i2c_intf.fifo_data.push(0x08);
        i2c_intf.fifo_data.push(0x09);
        i2c_intf.fifo_data.push(0x10);

        mem_intf.deq_en = false;
        for(uint16_t i = 0; i<32000; i++){
                eth_intf.tick();
                mem_intf.tick();
                i2c_intf.tick();
                eth_intf.clear_rx_fifo();
                tb->tick();
                
        }     

        mem_intf.deq_en = true;
        for(uint16_t i = 0; i<32000; i++){
                eth_intf.tick();
                mem_intf.tick();
                i2c_intf.tick();
                eth_intf.clear_rx_fifo();
                tb->tick();
                
        }       
}
