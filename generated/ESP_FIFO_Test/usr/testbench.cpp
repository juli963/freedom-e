
#include "Console_Defines.h"
#include "Ethernet.h"
#include "ESP_FIFO_Test.h"

int main(int argc, char **argv) {

        ESP_FIFO_Test_TB *tb = new ESP_FIFO_Test_TB();
        Ethernet eth_intf(tb);
        tb->opentrace("trace.vcd");
        printf("---------------------------------------------\n");
        tb->init();
        eth_intf.init();
        printf("\n");
       /* uint8_t arr[] = {0x7c ,0x2e ,0xbd ,0x1d ,0xf4 ,0x47 ,0xf0 ,0x2f ,0x74 ,0x1d ,0x83 ,0xa7 ,0x08 ,0x00 ,0x45 ,0x00
           ,0x00 ,0x34 ,0xb3 ,0x32 ,0x40 ,0x00 ,0x40 ,0x06 ,0x01 ,0x67 ,0xc0 ,0xa8 ,0x02 ,0x75 ,0xc0 ,0xa8
           ,0x02 ,0x65 ,0x81 ,0x0e ,0x1f ,0x49 ,0x5a ,0x81 ,0xf4 ,0xa4 ,0x04 ,0x53 ,0x9c ,0x32 ,0x80 ,0x10
           ,0x02 ,0x50 ,0x86 ,0x51 ,0x00 ,0x00 ,0x01 ,0x01 ,0x08 ,0x0a ,0xb8 ,0x67 ,0x42 ,0x7e ,0x00 ,0x74
           ,0x7e ,0xeb};
           */
        
        /*
        uint8_t arr[] = {0x01, 0x00, 0x5e ,0x7f ,0xff ,0xfb ,0x7c ,0x2e ,0xbd ,0x1d ,0xf4 ,0x47 ,0x08 ,0x00 ,0x45 ,0x00
                        ,0x00 ,0x40 ,0x00 ,0x00 ,0x40 ,0x00 ,0x04 ,0x11 ,0xc3 ,0xa4 ,0xc0 ,0xa8 ,0x02 ,0x65 ,0xef ,0xff
                        ,0xff ,0xfb ,0x27 ,0x75 ,0x27 ,0x75 ,0x00 ,0x2c ,0x53 ,0xe9 ,0x0a ,0x20 ,0x34 ,0x35 ,0x41 ,0x37
                        ,0x34 ,0x34 ,0x34 ,0x46 ,0x30 ,0x38 ,0x44 ,0x43 ,0x33 ,0x31 ,0x31 ,0x39 ,0x41 ,0x32 ,0x39 ,0x36
                        ,0x45 ,0x41 ,0x39 ,0x33 ,0x43 ,0x44 ,0x32 ,0x32 ,0x36 ,0x41 ,0x34 ,0x37 ,0x10 ,0x01
                        };

        for(uint8_t i = 0; i< sizeof(arr); i++){
                eth_intf.rx_data.push(arr[i]);
        }
        
        eth_intf.rx_length.push(sizeof(arr));
        eth_intf.clear_rx_fifo();
        */
       
        uint8_t ip[] = {0xC0, 0xA8, 0x02, 0xD4};
        uint8_t mac[] = {17,18,19,20,21,22};

        //eth_intf.createIP4Frame(ip, 0, ip, eth_intf.srcMAC, 17);
        //eth_intf.createUDPFrame(ip, 0, ip, mac, 15000, 16000);
        //eth_intf.createARPFrame(ip);
        uint8_t type = 0x10, ident = 0x01, cmd = 0x03, address = 0x1F;
        uint32_t data = 0x31323334;
        tb->rx_start_data = data;
        tb->tx_start_data = data;

        for(uint8_t i = 0; i<4; i++){
                eth_intf.createESPFrame(ip, mac, type, ident, cmd, address, data);
                data++;
        }


        for(uint16_t i = 0; i<16000; i++){
                eth_intf.tick();
                tb->tick();
                eth_intf.clear_rx_fifo();
        }        

        for(uint16_t i= 0; i<256; i++){
                if((i % 16) == 0){
                        tb->m_core->io_deq_Data = 1;
                }
                
                eth_intf.tick();
                tb->add_rx_fifo();
                tb->send_back();
                tb->tick();

                eth_intf.clear_rx_fifo();
                tb->check_rx_fifo_tick();
                
                
                tb->m_core->io_deq_Data = 0;
                tb->m_core->io_enq_Data = 0;        
        }
        

        

        for(uint16_t i = 0; i<16000; i++){
                eth_intf.tick();
                eth_intf.clear_rx_fifo();
                tb->tick();
                tb->m_core->io_deq_Data = 0;
                tb->m_core->io_enq_Data = 0;                
        }        

        for(uint16_t i = 0; i<16000; i++){
                tb->check_tx_fifo_tick();
        }
}
