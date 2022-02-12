
#include "Console_Defines.h"
#include "c8b10b_to_udp_test.h"
#include "Memory.h"
#include "Ethernet.h"
#include "Sniffer_8b10b_40.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

void create_testdata(uint8_t first, uint8_t second, uint8_t jump, uint8_t* data, uint8_t* isk, uint16_t length, Sniffer_8b10b_40_TB* tb );
void check_testdata(uint8_t* data, uint8_t* isk, uint16_t length, Ethernet* eth, std::ofstream* myfile);

int main(int argc, char **argv) {
    bool test_en[] = {
        false,   // Pre Test
        false,   // Jump to 16 short data
        false,   // Jump to 16 long data
        false,   // Jump to 16 more long data
        true,   // Go wild
        false,   // MGMT Overfill
        false    // Data Overfill
    };
    /*bool test_en[] = {
        true,   // Pre Test
        true,   // Jump to 16 short data
        true,   // Jump to 16 long data
        true,   // Jump to 16 more long data
        true,   // Go wild
        true,   // MGMT Overfill
        true    // Data Overfill
    };*/

    srand( time( NULL ) );

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

    data[2] = 0xFB;
    isk[2] = 0x01;
    uint8_t countmgmt = 0;  
    if(test_en[0]){
        //create_testdata(0 ,1 ,0 ,data ,isk ,sizeof(data) , &c8b10b_intf );
        create_testdata(0 ,1 ,0 ,data ,isk ,sizeof(data) , &c8b10b_intf );

        for(uint16_t i = 0; i<700; i++){
                eth_intf.tick();
                mem_intf.tick();
                c8b10b_intf.deq_rx_fifo();
                eth_intf.clear_rx_fifo();
                if(!eth_intf.fifo_mgmt.empty()){
                    check_testdata(&data[1], &isk[1], 1, &eth_intf, &myfile);

                    countmgmt++;
                }
                
                tb->tick();  
        }    
    }

    data[2] = 0xFA;
    isk[2] = 0x00;
    
    if(test_en[1]){
        printf("\n Trigger at Start and Stop Symbol \n");
        myfile << std::endl;
        myfile << "Trigger at Start and Stop Symbol" << std::endl;
        
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
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    };

    if(test_en[2]){
        printf("\n Trigger at Start and Stop Symbol 2 \n");
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

    uint8_t data3[] = {
        0xBC, 0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
        0xBC, 0x32, 0x11, 0x12, 0x02, 0x03, 0x04, 0x05, 
        0xAB, 0xAA, 0x55, 0x66, 0x88, 0x71, 0x72, 0x73, 
        0x41, 0x42, 0x43, 0x77, 0x44, 0x48, 0x71, 0x25, 
        0x91, 0x90, 0x89, 0x65, 0x89, 0x15, 0x27, 0x46, 
        0xAB, 0xAA, 0x55, 0x66, 0x88, 0x71, 0x72, 0x73, 
        0x17, 0xC1, 0xC2, 0xC3, 0xC4, 0xC7, 0xC8, 0xCA, 
        0xBC, 0x02, 0x10, 0x21, 0x22, 0xAB, 0xCD, 0xFB
    };
    uint8_t isk3[]= {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    };

    if(test_en[3]){
        printf("\n Trigger at Start and Stop Symbol 3\n");
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
                    create_testdata(first ,second ,jump ,data3 ,isk3 ,sizeof(data3) ,&c8b10b_intf );
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
                                check_testdata(&data3[1], &isk3[1], sizeof(data3)-2, &eth_intf, &myfile);
                            }else{
                                check_testdata(&data3[1], &isk3[1], sizeof(data3)-2-jump, &eth_intf, &myfile);
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
                                check_testdata(&data3[1], &isk3[1], sizeof(data3)-2, &eth_intf, &myfile);
                            }else{
                                check_testdata(&data3[1], &isk3[1], sizeof(data3)-2-jump, &eth_intf, &myfile);
                            }
                            countmgmt++;
                        }
                    }
                }
            }
        }
    }

    tb->m_core->io_first = 0;
    tb->m_core->io_second = 0;
    tb->m_core->io_jump = 0;

    if(test_en[4]){
        printf("\nGo Wild with Data ;) \n");
        myfile << std::endl << "Go Wild with Data" << std::endl;
        uint16_t length = 0;
        uint8_t data4[4096] = { 0 };
        uint8_t isk4[4096]= { 0 };
        char hex_string[3];

        for(uint16_t second = 0; second < 500; second++){
            tb->m_core->io_len = second;
            countmgmt = 0;
            length = rand() % 4090 + 3;
            printf("Go Wild, with len = %i, second = %i \n", length, second);
            myfile << "Go Wild, with len = " << std::to_string(length) << ", second = " << std::to_string(second) << std::endl;
            myfile << "{ ";
            for(uint16_t i = 0; i<length; i++){
                data4[i] = rand() % 255;
                isk4[i] = 0;
                if(i > 0 && i < (length-1)){
                    sprintf(hex_string, "%02X", data4[i]); 
                    myfile << hex_string << " ";
                }
            }
            myfile << "} " << std::endl;
            data4[length-1] = 0xFB;
            isk4[length-1] = 0x01;
            data4[0] = 0xBC;
            isk4[0] = 0x01;
            create_testdata(0 ,1 ,0 ,data4 ,isk4 ,length ,&c8b10b_intf ); // Data, 1 Fill Byte, Data, Data
            for(uint16_t i = 0; i < (length+200)*8*3; i++){
                eth_intf.tick();
                mem_intf.tick();
                c8b10b_intf.deq_rx_fifo();
                eth_intf.clear_rx_fifo();
                tb->tick();
                if(!eth_intf.fifo_mgmt.empty()){
                    if(countmgmt >= 2){
                        check_testdata(&data4[1], &isk4[1], length-2, &eth_intf, &myfile);
                    }else{
                        check_testdata(&data4[1], &isk4[1], length-2, &eth_intf, &myfile);
                    }
                    countmgmt++;
                }
                if (countmgmt > 2){
                    break;
                }
            }


            for(uint32_t i = 0; i < (length+200)*16*3; i++){
                eth_intf.tick();
                mem_intf.tick();
                c8b10b_intf.deq_rx_fifo();
                eth_intf.clear_rx_fifo();
                tb->tick();
                if(!eth_intf.fifo_mgmt.empty()){
                    if(countmgmt >= 2){
                        check_testdata(&data4[1], &isk4[1], length-2, &eth_intf, &myfile);
                    }else{
                        check_testdata(&data4[1], &isk4[1], length-2, &eth_intf, &myfile);
                    }
                    countmgmt++;
                }
                if (countmgmt > 2){
                    break;
                }
            }
        }
    }

    tb->m_core->io_first = 0;
    tb->m_core->io_second = 0;
    tb->m_core->io_jump++;
    if(test_en[5]){
        printf("\nOverfill MGMT FIFO \n");
        myfile << std::endl << "Overfill MGMT FIFO" << std::endl;
        for(uint16_t x = 0; x<20; x++){
            create_testdata(0 ,1 ,0 ,data3 ,isk3 ,sizeof(data3) ,&c8b10b_intf );
            for(uint8_t i = 0; i < 100; i++){
                eth_intf.tick();
                mem_intf.tick();
                c8b10b_intf.deq_rx_fifo();
                eth_intf.clear_rx_fifo();
                tb->tick();
                if(!eth_intf.fifo_mgmt.empty()){
                    if(countmgmt >= 2){
                        check_testdata(&data3[1], &isk3[1], sizeof(data3)-2, &eth_intf, &myfile);
                    }else{
                        check_testdata(&data3[1], &isk3[1], sizeof(data3)-2, &eth_intf, &myfile);
                    }
                    countmgmt++;
                }
            }
        }
        for(uint16_t x = 0; x<2000; x++)    {
            eth_intf.tick();
            mem_intf.tick();
            c8b10b_intf.deq_rx_fifo();
            eth_intf.clear_rx_fifo();
            tb->tick();
            if(!eth_intf.fifo_mgmt.empty()){
                if(countmgmt >= 2){
                    check_testdata(&data3[1], &isk3[1], sizeof(data3)-2, &eth_intf, &myfile);
                }else{
                    check_testdata(&data3[1], &isk3[1], sizeof(data3)-2, &eth_intf, &myfile);
                }
                countmgmt++;
            }
        }
    }

    tb->m_core->io_first = 0;
    tb->m_core->io_second = 0;
    tb->m_core->io_jump++;
    if(test_en[6]){
        printf("Overfill Data FIFO \n");
        myfile << std::endl << "Overfill Data FIFO" << std::endl;
        uint16_t longdat = 3*(2048/sizeof(data));
        printf("Len %i\n", longdat);
        isk3[sizeof(data3)-1] = 0;
        for(uint16_t x = 0; x< longdat; x++){
            eth_intf.tick();
            mem_intf.tick();
            c8b10b_intf.deq_rx_fifo();
            eth_intf.clear_rx_fifo();
            tb->tick();
            create_testdata(0 ,1 ,0 ,data3 ,isk3 ,sizeof(data3) ,&c8b10b_intf );
            if(!eth_intf.fifo_mgmt.empty()){
                if(countmgmt >= 2){
                    check_testdata(&data3[1], &isk3[1], sizeof(data3)-2, &eth_intf, &myfile);
                }else{
                    check_testdata(&data3[1], &isk3[1], sizeof(data3)-2, &eth_intf, &myfile);
                }
                countmgmt++;
            }

        }
        isk3[sizeof(data3)-1] = 0x01;

        for(uint16_t x = 0; x<2000; x++)    {
            eth_intf.tick();
            mem_intf.tick();
            c8b10b_intf.deq_rx_fifo();
            eth_intf.clear_rx_fifo();
            tb->tick();
            if(!eth_intf.fifo_mgmt.empty()){
                if(countmgmt >= 2){
                    check_testdata(&data3[1], &isk3[1], sizeof(data3)-2, &eth_intf, &myfile);
                }else{
                    check_testdata(&data3[1], &isk3[1], sizeof(data3)-2, &eth_intf, &myfile);
                }
                countmgmt++;
            }
        }
    }

}

void create_testdata(uint8_t first, uint8_t second, uint8_t jump, uint8_t* data, uint8_t* isk, uint16_t length, Sniffer_8b10b_40_TB* tb ){
    /*
        Valid Values for first [0..3]
        Valid Values for second[0..4]
        Valid Values for jump[0..3]
    */

    uint8_t arrdat_f[first] = {0};
    uint8_t arrk_f[first] = {0};
    for(uint16_t i = 0; i<first; i++){
        arrdat_f[i] = 0x50 + i;
    }

    uint8_t arrdat_s[second] = {0};
    uint8_t arrk_s[second] = {0};
    for(uint16_t i = 0; i<second; i++){
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



void check_testdata(uint8_t* data, uint8_t* isk, uint16_t length, Ethernet* eth, std::ofstream* myfile){
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

    uint8_t temp, tempk = 0;
    for(uint16_t i = 0; i< mgmtlen; i++){
        temp = eth->fifo_data.front().data;
        tempk = eth->fifo_data.front().isk;
        eth->fifo_data.pop();

        sprintf(hex_string, "%02X", temp); 
        *myfile << hex_string << " ";

        if( temp != data[i] ){
            CRED
            printf("Data FIFO idx %i wrong\n", i);
            error = true;
            CDEFAULT
        }
        if( tempk != isk[i]){
            CRED
            printf("IsK FIFO idx %i wrong\n", i);
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
        CRED
        printf("Data Error ");
        CDEFAULT
    }

    if( mgmtlen != length ){
        *myfile << " MGMT Len neq Data Len" ;
        CRED
        printf("Data Length != MGMT Length ");
        CDEFAULT
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