
#include "Console_Defines.h"
#include "Sniffer_8b10b_40.h"
#include <stdlib.h>     /* srand, rand */
#include <iostream>
#include <fstream>
#include <string.h>

void create_testdata(uint8_t first, uint8_t second, uint8_t jump, uint8_t* data, uint8_t* isk, uint8_t length, Sniffer_8b10b_40_TB* tb );
void check_testdata(uint8_t* data, uint8_t* isk, uint8_t length, Sniffer_8b10b_40_TB* tb, std::ofstream* myfile);

int main(int argc, char **argv) {

    Sniffer_8b10b_40_TB *tb = new Sniffer_8b10b_40_TB();

    std::ofstream myfile;
    myfile.open ("example.txt");

    tb->opentrace("trace.vcd");
    tb->init();

    Sniffer_8b10b_40_TB::s8b10b temp[2];
    temp[0].data = 0xBC;
    temp[0].isk = 1;
    temp[1].data = 0xFB;
    temp[1].isk = 1;

    tb->enable_trigger_code(temp[0], temp[1], Sniffer_8b10b_40_TB::estart_stop, 0);
    tb->sel_gtp(0);
    /*
        uint8_t data[] = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xBC, 0x02,
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xBC, 0x02,
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xBC, 0x02,
            0x11, 0x12, 0x02, 0x03, 0x04, 0x05, 0xBC, 0x02,
            0x00, 0x01, 0x02, 0xFB, 0xBC, 0x05, 0x07, 0x02,
            0x00, 0x01, 0x02, 0xFA, 0xBC, 0x05, 0x07, 0x02,
            0x00, 0x01, 0x02, 0xFE, 0xBC, 0x05, 0x07, 0x02,
            0x00, 0x01, 0x02, 0xFB, 0xBC, 0x05, 0x07, 0x02,
            0x00, 0x01, 0x02, 0xFB, 0xBC, 0x05, 0x07, 0x02
        };
        uint8_t isk[]= {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

        };
    */

    uint8_t data[] = {
        0xBC, 0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
        0xBC, 0x32, 0x11, 0x12, 0x02, 0x03, 0x04, 0x05, 
        0xBC, 0x02, 0x10, 0x21, 0x22, 0xAB, 0xCD, 0xFB
    };
    uint8_t isk[]= {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    };


    
    printf("Trigger at Start and Stop Symbol \n");
    myfile << "Trigger at Start and Stop Symbol" << std::endl;
    for(uint8_t first = 0; first < 4; first++){
        tb->m_core->io_first = first;
        for(uint8_t second = 0; second < 5; second++){
            tb->m_core->io_second = second;
            for(uint8_t jump = 0; jump < 4; jump++){
                tb->m_core->io_jump = jump;
                printf("Do Test, first = %i second = %i jump = %i \n", first, second, jump);
                myfile << "Test State: First=" << std::to_string(first) << ", Second=" << std::to_string(second) << ", Jump=" << std::to_string(jump) << std::endl;
                create_testdata(first ,second ,jump ,data ,isk ,sizeof(data) ,tb );
                for(uint8_t i = 0; i < 128; i++){
                    if(i > 100 && tb->rx_fifo.empty()){
                        break;
                    }
                    tb->tick();
                }

                for(uint16_t i = 0; i < 25; i++){
                    tb->tick();
                }

                
                for(uint16_t i = 0; i < 128; i++){
                    tb->deq_rx_fifo();
                    if(i > 100 && tb->m_core->io_data_empty_0 == 1){
                        break;
                    }
                    tb->tick();
                }
                for(uint16_t i = 0; i < 25; i++){
                    tb->tick();
                }
                check_testdata(data, isk, sizeof(data) ,tb, &myfile);
                for(uint16_t i = 0; i < 25; i++){
                    tb->tick();
                }
            }
        }
    }

    printf("Trigger at Start Symbol and Length \n");
    myfile << "Trigger at Start Symbol and Length" << std::endl;
    for(uint8_t len = 1; len < 20; len+=1){ 
        uint8_t arrdat[len+8] = {0};
        uint8_t arrk[len+8] = {0};
        arrdat[0] = 0xBC;
        arrk[0] = 0x01;
        for(uint8_t n = 1; n<len+8; n++){
            arrdat[n] = n + 0x10;
            arrk[n] = 0x00;
        }

        tb->enable_trigger_length(temp[0], len, Sniffer_8b10b_40_TB::elength, 0);
        for(uint8_t first = 0; first < 4; first++){
            tb->m_core->io_first = first;
            for(uint8_t second = 0; second < 5; second++){
                tb->m_core->io_second = second;
                //for(uint8_t jump = 0; jump < 4; jump++){
                    tb->m_core->io_jump = len;
                    printf("Do Test, len = %i, first = %i second = %i \n", len, first, second);
                    myfile << "Test State: Len= "<< std::to_string(len) << ", First=" << std::to_string(first) << ", Second=" << std::to_string(second) << std::endl;
                    create_testdata(first ,second ,0 ,arrdat ,arrk ,sizeof(arrdat) ,tb );
                    for(uint8_t i = 0; i < 128; i++){
                        if(i > 100 && tb->rx_fifo.empty()){
                            break;
                        }
                        tb->tick();
                    }

                    for(uint16_t i = 0; i < 25; i++){
                        tb->tick();
                    }

                    
                    for(uint16_t i = 0; i < 128; i++){
                        tb->deq_rx_fifo();
                        if(i > 100 && tb->m_core->io_data_empty_0 == 1){
                            break;
                        }
                        tb->tick();
                    }
                    for(uint16_t i = 0; i < 25; i++){
                        tb->tick();
                    }
                    check_testdata(arrdat, arrk, sizeof(arrdat) ,tb, &myfile);
                    for(uint16_t i = 0; i < 25; i++){
                        tb->tick();
                    }
                //}
            }
        }
    }


    //myfile << "Writing this to a file2.\n";
    myfile.close();

/*
    for(uint16_t i = 0; i < 5000; i++){
        tb->tick();
    }
*/
   /* tb->deq_hw_fifo(1, 0);
    for(uint16_t i = 0; i < 5000; i++){
        tb->tick();
    }*/

}

void check_testdata(uint8_t* data, uint8_t* isk, uint8_t length, Sniffer_8b10b_40_TB* tb, std::ofstream* myfile){
    uint16_t templength = 0;
    uint8_t idx = 1;
    bool dodeq = true;
    bool waitvalid = false;
    bool error = false;
    while(true){
        
        templength = 0;
        if (dodeq){
            tb->m_core->io_mgmt_deq_0_ready = 1;
            dodeq = false;
        }else{
            tb->m_core->io_mgmt_deq_0_ready = 0;
        }
        if( tb->m_core->io_mgmt_empty_0 == 1 && tb->m_core->io_mgmt_deq_0_valid == 0 ){
            break;
        }else if(tb->m_core->io_data_empty_0 == 1){
            CRED
            printf("Data Fifo empty before Management\n");
            error = true;
            CDEFAULT
        }else{
            
            if(tb->m_core->io_mgmt_deq_0_valid > 0){
                templength = tb->m_core->io_mgmt_deq_0_bits_length;
                if(templength > length){
                    CRED
                    printf("MGMT Length is wrong, no Data Checks will be done.\n");
                    error = true;
                    CDEFAULT
                }
                *myfile << "{" << " ";
                //printf("Dequeue Data FIFO \n");
                waitvalid = true;
                while(true){    // DATA LOOP
                    if( tb->m_core->io_data_empty_0 == 1 && tb->m_core->io_data_deq_0_valid == 0 && !(templength < 4 && tb->m_core->io_mgmt_deq_0_bits_hasData > 0) ){
                        CRED
                        printf("Data Fifo empty\n");
                        error = true;
                        CDEFAULT
                        break;
                    }else{
                        if(waitvalid){
                            waitvalid = false;
                            if(templength < 4 && tb->m_core->io_mgmt_deq_0_bits_hasData > 0){
                                // Check MGMT Data here
                                char hex_string[3];
                                if(idx <= (tb->m_core->io_mgmt_deq_0_bits_length) ){
                                    if(!error && (tb->m_core->io_mgmt_deq_0_bits_data_0 != data[idx] || tb->m_core->io_mgmt_deq_0_bits_isk_0 != isk[idx]) ){
                                        CRED
                                        printf("MGMT FIFO idx %i wrong\n", idx);
                                        error = true;
                                        CDEFAULT
                                    }
                                    sprintf(hex_string, "%02X", tb->m_core->io_mgmt_deq_0_bits_data_0); 
                                    *myfile << hex_string << " ";
                                }
                                idx++;
                                if(idx <= (tb->m_core->io_mgmt_deq_0_bits_length) ){
                                    if(!error && (tb->m_core->io_mgmt_deq_0_bits_data_1 != data[idx] || tb->m_core->io_mgmt_deq_0_bits_isk_1 != isk[idx]) ){
                                        CRED
                                        printf("MGMT FIFO idx %i wrong\n", idx);
                                        error = true;
                                        CDEFAULT
                                    }
                                    sprintf(hex_string, "%02X", tb->m_core->io_mgmt_deq_0_bits_data_1); 
                                    *myfile << hex_string << " ";
                                }
                                idx++;
                                if(idx <= (tb->m_core->io_mgmt_deq_0_bits_length) ){
                                    if(!error && (tb->m_core->io_mgmt_deq_0_bits_data_2 != data[idx] || tb->m_core->io_mgmt_deq_0_bits_isk_2 != isk[idx]) ){
                                        CRED
                                        printf("MGMT FIFO idx %i wrong\n", idx);
                                        error = true;
                                        CDEFAULT
                                    }
                                    sprintf(hex_string, "%02X", tb->m_core->io_mgmt_deq_0_bits_data_2); 
                                    *myfile << hex_string << " ";
                                }
                                idx++;
                                break;
                            }
                            tb->m_core->io_data_deq_0_ready = 1;
                        }else{
                            char hex_string[3];
                            tb->m_core->io_data_deq_0_ready = 0;
                            if(tb->m_core->io_data_deq_0_valid > 0){

                                // Check Data here
                                if(idx <= (tb->m_core->io_mgmt_deq_0_bits_length) ){
                                    if(!error && (tb->m_core->io_data_deq_0_bits_data_0 != data[idx] || tb->m_core->io_data_deq_0_bits_isk_0 != isk[idx]) ){
                                        CRED
                                        printf("Data FIFO idx %i wrong\n", idx);
                                        error = true;
                                        CDEFAULT
                                    }
                                    sprintf(hex_string, "%02X", tb->m_core->io_data_deq_0_bits_data_0); 
                                    *myfile << hex_string << " ";
                                    //*myfile <<  tb->m_core->io_data_deq_0_bits_data_0 << " ";
                                }
                                idx++;
                                if(idx <= (tb->m_core->io_mgmt_deq_0_bits_length) ){
                                    if(!error && (tb->m_core->io_data_deq_0_bits_data_1 != data[idx] || tb->m_core->io_data_deq_0_bits_isk_1 != isk[idx]) ){
                                        CRED
                                        printf("Data FIFO idx %i wrong\n", idx);
                                        error = true;
                                        CDEFAULT
                                    }
                                    sprintf(hex_string, "%02X", tb->m_core->io_data_deq_0_bits_data_1); 
                                    *myfile << hex_string << " ";
                                }
                                idx++;
                                if(idx <= (tb->m_core->io_mgmt_deq_0_bits_length) ){
                                    if(!error && (tb->m_core->io_data_deq_0_bits_data_2 != data[idx] || tb->m_core->io_data_deq_0_bits_isk_2 != isk[idx]) ){
                                        CRED
                                        printf("Data FIFO idx %i wrong\n", idx);
                                        error = true;
                                        CDEFAULT
                                    }
                                    sprintf(hex_string, "%02X", tb->m_core->io_data_deq_0_bits_data_2); 
                                    *myfile << hex_string << " ";
                                }
                                idx++;
                                if(idx <= (tb->m_core->io_mgmt_deq_0_bits_length) ){
                                    if(!error && (tb->m_core->io_data_deq_0_bits_data_3 != data[idx] || tb->m_core->io_data_deq_0_bits_isk_3 != isk[idx]) ){
                                        CRED
                                        printf("Data FIFO idx %i wrong\n", idx);
                                        error = true;
                                        CDEFAULT
                                    }
                                    sprintf(hex_string, "%02X", tb->m_core->io_data_deq_0_bits_data_3); 
                                    *myfile << hex_string << " ";
                                }
                                idx++;
                                if(idx <= (tb->m_core->io_mgmt_deq_0_bits_length) ){
                                    if(!error && (tb->m_core->io_data_deq_0_bits_data_4 != data[idx] || tb->m_core->io_data_deq_0_bits_isk_4 != isk[idx]) ){
                                        CRED
                                        printf("Data FIFO idx %i wrong\n", idx);
                                        error = true;
                                        CDEFAULT
                                    }
                                    sprintf(hex_string, "%02X", tb->m_core->io_data_deq_0_bits_data_4); 
                                    *myfile << hex_string << " ";
                                }
                                idx++;
                                if(idx <= (tb->m_core->io_mgmt_deq_0_bits_length) ){
                                    if(!error && (tb->m_core->io_data_deq_0_bits_data_5 != data[idx] || tb->m_core->io_data_deq_0_bits_isk_5 != isk[idx]) ){
                                        CRED
                                        printf("Data FIFO idx %i wrong\n", idx);
                                        error = true;
                                        CDEFAULT
                                    }
                                    sprintf(hex_string, "%02X", tb->m_core->io_data_deq_0_bits_data_5); 
                                    *myfile << hex_string << " ";
                                }
                                idx++;
                                if(idx <= (tb->m_core->io_mgmt_deq_0_bits_length) ){
                                    if(!error && (tb->m_core->io_data_deq_0_bits_data_6 != data[idx] || tb->m_core->io_data_deq_0_bits_isk_6 != isk[idx]) ){
                                        CRED
                                        printf("Data FIFO idx %i wrong\n", idx);
                                        error = true;
                                        CDEFAULT
                                    }
                                    sprintf(hex_string, "%02X", tb->m_core->io_data_deq_0_bits_data_6); 
                                    *myfile << hex_string << " ";
                                }
                                idx++;
                                if(idx <= (tb->m_core->io_mgmt_deq_0_bits_length) ){
                                    if(!error && (tb->m_core->io_data_deq_0_bits_data_7 != data[idx] || tb->m_core->io_data_deq_0_bits_isk_7 != isk[idx]) ){
                                        CRED
                                        printf("Data FIFO idx %i wrong\n", idx);
                                        error = true;
                                        CDEFAULT
                                    }
                                    sprintf(hex_string, "%02X", tb->m_core->io_data_deq_0_bits_data_7); 
                                    *myfile << hex_string << " ";
                                }
                                idx++;
                                waitvalid = true;

                                if(templength >= 8){
                                    templength = templength - 8;
                                }else{
                                    break;
                                }

                                
                            }
                        }
                        tb->tick();
                    }
                }
                *myfile << "}" ;
                if(error){
                    *myfile << " ERROR!!!!!!!!!!!!!!!!!!" ;
                }
                *myfile << std::endl;
                idx = 1;
                dodeq = true;
            }   
        }
        tb->tick();
    }
    tb->m_core->io_mgmt_deq_0_ready = 0;
    tb->m_core->io_data_deq_0_ready = 0;
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


/**
 *     val io = IO(new Bundle{
        val data_in = Vec(Channels, Flipped(ValidIO(new B_8b10b(4))))

        val regs = new Bundle{
            val enable = Input(Bool())
            val gtp_sel = Input(UInt(2.W))
            val rate = Input(UInt(3.W))
            val scrambler_en = Input(Bool())
            val trigger_mode = Input(Vec(num_triggers, UInt(2.W)))

            val trigger_start = Vec(num_triggers, Flipped(new B_8b10b(1)))
            val trigger_stop = Vec(num_triggers, Flipped(new B_8b10b(1)))
        }

        val data_deq = Vec(num_triggers, DecoupledIO(new RAW_8b10b))  // Entry per Trigger
        val mgmt_deq = Vec(num_triggers, DecoupledIO(new MGMT_8b10b))
        val mem_clock = Input(Clock())
 * 
 */

