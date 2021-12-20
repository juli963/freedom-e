
#include "Console_Defines.h"
#include "Ethernet.h"
#include "Memory.h"
#include "dram_to_udp_test.h"
#include <stdlib.h>

int main(int argc, char **argv) {

        dram_to_udp_test_TB *tb = new dram_to_udp_test_TB();
        Ethernet eth_intf(tb);
        Memory mem_intf(tb, 10);
        tb->opentrace("trace.vcd");
        tb->init();
        printf("---------------------------------------------\n");
        tb->init();
        eth_intf.init();
        mem_intf.init();
        printf("\n");

        tb->m_core->io_eth_params_MAC = 0xFFAFABACAD;
        tb->m_core->io_eth_params_IP = 0xC0A80206;
        
        //(uint8_t type, uint32_t flags, uint16_t length, uint8_t* data, uint64_t timestamp_start, uint64_t timestamp_end){
        const uint8_t len = 32;
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
        }
        

        for(uint16_t i = 0; i<32000; i++){
                eth_intf.tick();
                mem_intf.tick();
                eth_intf.clear_rx_fifo();
                tb->tick();
                
        }       

}
