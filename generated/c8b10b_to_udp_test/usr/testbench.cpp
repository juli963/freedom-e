
#include "Console_Defines.h"
#include "c8b10b_to_udp_test.h"
#include "Memory.h"
#include "Ethernet.h"
#include "Sniffer_8b10b_40.h"

void create_testdata(uint8_t first, uint8_t second, uint8_t jump, uint8_t* data, uint8_t* isk, uint8_t length, Sniffer_8b10b_40_TB* tb );

int main(int argc, char **argv) {

        c8b10b_to_udp_test_TB *tb = new c8b10b_to_udp_test_TB();
        Ethernet eth_intf(tb);
        Sniffer_8b10b_40_TB c8b10b_intf(tb->m_core);
        Memory mem_intf(tb, 10);
        tb->opentrace("trace.vcd");
        tb->init();
        eth_intf.init();
        c8b10b_intf.init();
        mem_intf.init();

        mem_intf.deq_en = true;
        tb->m_core->io_eth_params_MAC = 0xFFAFABACAD;
        tb->m_core->io_eth_params_IP = 0xC0A80206;

        uint8_t data[] = {
                0xBC, 0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
                0xBC, 0x32, 0x11, 0x12, 0x02, 0x03, 0x04, 0x05, 
                0xBC, 0x02, 0x10, 0x21, 0x22, 0xAB, 0xCD, 0xFB
        };
        uint8_t isk[]= {
                0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        };

        Sniffer_8b10b_40_TB::s8b10b temp[2];
        temp[0].data = 0xBC;
        temp[0].isk = 1;
        temp[1].data = 0xFB;
        temp[1].isk = 1;

        tb->m_core->io_regs_enable = 1;

        c8b10b_intf.enable_trigger_code(temp[0], temp[1], Sniffer_8b10b_40_TB::estart_stop, 0);
        c8b10b_intf.sel_gtp(0);

        create_testdata(0 ,1 ,0 ,data ,isk ,sizeof(data) , &c8b10b_intf );

        for(uint16_t i = 0; i<10000; i++){
                eth_intf.tick();
                mem_intf.tick();
                c8b10b_intf.deq_rx_fifo();
                eth_intf.clear_rx_fifo();
                tb->tick();  
        }     
}

void create_testdata(uint8_t first, uint8_t second, uint8_t jump, uint8_t* data, uint8_t* isk, uint8_t length, Sniffer_8b10b_40_TB* tb ){
    /*
        Valid Values for first [0..3]
        Valid Values for second[0..4]
        Valid Values for jump[0..3]
    */

    uint8_t arrdat_f[first] = {0};
    uint8_t arrk_f[first] = {0};
    for(uint8_t i = 0; i<first; i++){
        arrdat_f[i] = 0x50 + i;
    }

    uint8_t arrdat_s[second] = {0};
    uint8_t arrk_s[second] = {0};
    for(uint8_t i = 0; i<second; i++){
        arrdat_s[i] = 0x60 + i;
    }

    uint8_t arrdat[length-jump] = {0};
    uint8_t arrk[length-jump] = {0};
    std::memcpy(arrdat, data, length-jump);
    std::memcpy(arrk, isk, length-jump);
    arrdat[length-jump-1] = data[length-1];
    arrk[length-jump-1] = isk[length-1];

    //printf("Add FrontData \n");
    tb->enq_rx_fifo(arrdat_f, arrk_f, sizeof(arrdat_f));
    //printf("Add Triggerdata \n");
    tb->enq_rx_fifo(arrdat, arrk, sizeof(arrdat));
    //printf("Add FillData \n");
    tb->enq_rx_fifo(arrk_s, arrk_s, sizeof(arrk_s));
    //printf("Add Realdata \n");
    tb->enq_rx_fifo(arrdat, arrk, sizeof(arrdat));
    //tb->enq_rx_fifo(data, isk, length);
    tb->enq_rx_fifo(arrk_s, arrk_s, sizeof(arrk_s));
    tb->enq_rx_fifo(data, isk, length);
}