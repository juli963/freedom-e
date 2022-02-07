
#include "Console_Defines.h"
#include "c8b10b_to_udp_test.h"
#include "Memory.h"
#include "Ethernet.h"
#include "Sniffer_8b10b_40.h"
#include <iostream>
#include <fstream>
#include <string.h>

void create_testdata(uint8_t first, uint8_t second, uint8_t jump, uint8_t* data, uint8_t* isk, uint8_t length, Sniffer_8b10b_40_TB* tb );
void check_testdata(uint8_t* data, uint8_t* isk, uint8_t length, Ethernet* eth, std::ofstream* myfile);

int main(int argc, char **argv) {

    std::ofstream myfile;
    myfile.open ("example.txt");

    c8b10b_to_udp_test_TB *tb = new c8b10b_to_udp_test_TB();
    Ethernet eth_intf(tb);
    Sniffer_8b10b_40_TB c8b10b_intf(tb->m_core);
    Memory mem_intf(tb, 10);
    tb->opentrace("trace.vcd");
    tb->init();
    eth_intf.init();
    eth_intf.print_output = false;
    c8b10b_intf.init();
    mem_intf.init();

    mem_intf.deq_en = true;
    tb->m_core->io_eth_params_MAC = 0xFFAFABACAD;
    tb->m_core->io_eth_params_IP = 0xC0A80206;

    uint8_t data[] = {
        0xBC, 0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
        0xBC, 0x32, 0x11, 0x12, 0x02, 0x03, 0x04, 0x05, 
        /*0xAB, 0xAA, 0x55, 0x66, 0x88, 0x71, 0x72, 0x73, 
        0x41, 0x42, 0x43, 0x77, 0x44, 0x48, 0x71, 0x25, 
        0x91, 0x90, 0x89, 0x65, 0x89, 0x15, 0x27, 0x46, */
        0xBC, 0x02, 0x10, 0x21, 0x22, 0xAB, 0xCD, 0xFB
    };
    uint8_t isk[]= {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,*/
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    };

    //02 00 01 02 03 04 05 00 03 04 05 00 BC 32 11 12

    //02 00 01 02 03 04 05 00 BC 32 11 12 02 03 00 01
    //02 10 21 22 AB CD FB 40 BC 00 00 00 00 00 00 00 

    Sniffer_8b10b_40_TB::s8b10b temp[2];
    temp[0].data = 0xBC;
    temp[0].isk = 1;
    temp[1].data = 0xFB;
    temp[1].isk = 1;

    tb->m_core->io_regs_enable = 1;

    c8b10b_intf.enable_trigger_code(temp[0], temp[1], Sniffer_8b10b_40_TB::estart_stop, 0);
    c8b10b_intf.sel_gtp(0);

    create_testdata(0 ,1 ,0 ,data ,isk ,sizeof(data) , &c8b10b_intf );

    for(uint16_t i = 0; i<700; i++){
            eth_intf.tick();
            mem_intf.tick();
            c8b10b_intf.deq_rx_fifo();
            eth_intf.clear_rx_fifo();
            if(!eth_intf.fifo_mgmt.empty()){
                check_testdata(&data[1], &isk[1], sizeof(data)-2, &eth_intf, &myfile);
            }
            
            tb->tick();  
    }    

    printf("\n Trigger at Start and Stop Symbol \n");
    myfile << std::endl;
    myfile << "Trigger at Start and Stop Symbol" << std::endl;
    uint8_t countmgmt = 0;
    for(uint8_t first = 0; first < 4; first++){
        tb->m_core->io_first = first;
        for(uint8_t second = 0; second < 5; second++){
            tb->m_core->io_second = second;
            for(uint8_t jump = 0; jump < 20; jump++){
                tb->m_core->io_jump = jump;
                countmgmt = 0;
                printf("Do Test, first = %i second = %i jump = %i \n", first, second, jump);
                myfile << "Test State: First=" << std::to_string(first) << ", Second=" << std::to_string(second) << ", Jump=" << std::to_string(jump) << std::endl;
                create_testdata(first ,second ,jump ,data ,isk ,sizeof(data) ,&c8b10b_intf );
                for(uint16_t i = 0; i < 350; i++){
                    eth_intf.tick();
                    mem_intf.tick();
                    c8b10b_intf.deq_rx_fifo();
                    eth_intf.clear_rx_fifo();
                    tb->tick();
                    if(!eth_intf.fifo_mgmt.empty()){
                        if(countmgmt >= 2){
                            check_testdata(&data[1], &isk[1], sizeof(data)-2, &eth_intf, &myfile);
                        }else{
                            check_testdata(&data[1], &isk[1], sizeof(data)-2-jump, &eth_intf, &myfile);
                        }
                        countmgmt++;
                    }
                }


                for(uint16_t i = 0; i < 350; i++){
                    eth_intf.tick();
                    mem_intf.tick();
                    c8b10b_intf.deq_rx_fifo();
                    eth_intf.clear_rx_fifo();
                    tb->tick();
                    if(!eth_intf.fifo_mgmt.empty()){
                        if(countmgmt >= 2){
                            check_testdata(&data[1], &isk[1], sizeof(data)-2, &eth_intf, &myfile);
                        }else{
                            check_testdata(&data[1], &isk[1], sizeof(data)-2-jump, &eth_intf, &myfile);
                        }
                        countmgmt++;
                    }
                }
            }
        }
    }

    uint8_t data2[] = {
        0xBC, 0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
        0xBC, 0x32, 0x11, 0x12, 0x02, 0x03, 0x04, 0x05, 
        0xAB, 0xAA, 0x55, 0x66, 0x88, 0x71, 0x72, 0x73, 
        0x41, 0x42, 0x43, 0x77, 0x44, 0x48, 0x71, 0x25, 
        0x91, 0x90, 0x89, 0x65, 0x89, 0x15, 0x27, 0x46, 
        0xBC, 0x02, 0x10, 0x21, 0x22, 0xAB, 0xCD, 0xFB
    };
    uint8_t isk2[]= {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    };

    printf("\n Trigger at Start and Stop Symbol \n");
    myfile << std::endl;
    myfile << "Trigger at Start and Stop Symbol" << std::endl;
    countmgmt = 0;
    for(uint8_t first = 0; first < 4; first++){
        tb->m_core->io_first = first;
        for(uint8_t second = 0; second < 5; second++){
            tb->m_core->io_second = second;
            for(uint8_t jump = 0; jump < 20; jump++){
                tb->m_core->io_jump = jump;
                countmgmt = 0;
                printf("Do Test, first = %i second = %i jump = %i \n", first, second, jump);
                myfile << "Test State: First=" << std::to_string(first) << ", Second=" << std::to_string(second) << ", Jump=" << std::to_string(jump) << std::endl;
                create_testdata(first ,second ,jump ,data2 ,isk2 ,sizeof(data2) ,&c8b10b_intf );
                for(uint16_t i = 0; i < 350; i++){
                    /*if(i > 700){
                        break;
                    }*/
                    eth_intf.tick();
                    mem_intf.tick();
                    c8b10b_intf.deq_rx_fifo();
                    eth_intf.clear_rx_fifo();
                    tb->tick();
                    if(!eth_intf.fifo_mgmt.empty()){
                        if(countmgmt >= 2){
                            check_testdata(&data2[1], &isk2[1], sizeof(data2)-2, &eth_intf, &myfile);
                        }else{
                            check_testdata(&data2[1], &isk2[1], sizeof(data2)-2-jump, &eth_intf, &myfile);
                        }
                        countmgmt++;
                    }
                }


                for(uint16_t i = 0; i < 350; i++){
                    eth_intf.tick();
                    mem_intf.tick();
                    c8b10b_intf.deq_rx_fifo();
                    eth_intf.clear_rx_fifo();
                    tb->tick();
                    if(!eth_intf.fifo_mgmt.empty()){
                        if(countmgmt >= 2){
                            check_testdata(&data2[1], &isk2[1], sizeof(data2)-2, &eth_intf, &myfile);
                        }else{
                            check_testdata(&data2[1], &isk2[1], sizeof(data2)-2-jump, &eth_intf, &myfile);
                        }
                        countmgmt++;
                    }
                }
            }
        }
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



void check_testdata(uint8_t* data, uint8_t* isk, uint8_t length, Ethernet* eth, std::ofstream* myfile){
    uint16_t templength = 0;
    bool error = false;
    char hex_string[3];

    uint16_t mgmtlen = eth->fifo_mgmt.front();
    eth->fifo_mgmt.pop();
    /*
    CRED
    printf("Data Fifo empty before Management\n");
    error = true;
    CDEFAULT

    CRED
    printf("MGMT Length is wrong, no Data Checks will be done.\n");
    CDEFAULT
    */

    

    *myfile << "{" << " ";

    uint8_t temp = 0;
    for(uint16_t i = 0; i< mgmtlen; i++){
        temp = eth->fifo_data.front().data;
        eth->fifo_data.pop();

        sprintf(hex_string, "%02X", temp); 
        *myfile << hex_string << " ";

        if( temp != data[i] ){
            CRED
            printf("Data FIFO idx %i wrong\n", i);
            error = true;
            CDEFAULT
        }
        /*if(i%8 == 0 && i > 0){
            *myfile << std::endl;
        }*/
    }
    *myfile << "}" ;

    if(error){
        *myfile << " Data Error" ;
        //printf("Data Length != MGMT Length \n");
    }

    if( mgmtlen != length ){
        *myfile << " MGMT Len neq Data Len" ;
        //printf("Data Length != MGMT Length \n");
        error = true;
    }

    if(error){
        *myfile << " ERROR!!!!!!!!!!!!!!!!!!" ;
    }
    *myfile << std::endl;

            
            
            



            

    if( error ){
        CRED
        printf("Error Found!\n");
        CDEFAULT
    }else{
        CGREEN
        printf("Test successful\n");
        CDEFAULT
    }
    //printf("Error Found:");
    //printf("%s", error ? "true" : "false");
    //printf("\n");
    
}