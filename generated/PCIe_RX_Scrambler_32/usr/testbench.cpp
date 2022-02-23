
#include "Console_Defines.h"
#include "PCIe_RX_Scrambler_32.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

void create_test(uint8_t first, uint8_t second, uint8_t third, uint8_t end, uint8_t* pData, uint8_t* pisk);
void do_test(PCIe_RX_Scrambler_32_TB *tb, std::ofstream* myfile, uint8_t* data, uint8_t* isk, uint16_t length);

int main(int argc, char **argv) {

        PCIe_RX_Scrambler_32_TB *tb = new PCIe_RX_Scrambler_32_TB();

        tb->opentrace("trace.vcd");
        tb->init();


        std::ofstream myfile;
        myfile.open ("example.txt");

        //myfile << "Test State: First=" << std::to_string(first) << ", Second=" << std::to_string(second) << ", Jump=" << std::to_string(jump) << std::endl;

        //char hex_string[3];
        //sprintf(hex_string, "%02X", temp); 
        //*myfile << hex_string << " ";

/*
        uint8_t data1[] = {
                        0x00, 0x00, 0x00, 0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x45,
                        0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45,
                        //0x00, 
                        
                        0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x45,
                        0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45,

                        0x00, 0x00,

                        0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x45,
                        0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45,

                        0x1C, 0x00, 0x1C, 0x00, 0x1C, 0x1C, 0x00, 0x1C,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        uint8_t isk1[] = {      
                        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        //0x00, 
                        
                        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                        0x00, 0x00,

                        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                        0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        tb->m_core->io_enable = 1;

        do_test(tb, &myfile, data1, isk1, sizeof(data1));



        uint8_t data2[] = {
                        0x00, 0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x45,
                        0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45,
                        0x00, 0x00, 0x00,
                        
                        0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x45,
                        0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45,

                        0x1C, 0x00, 0x1C, 0x00, 0x1C, 0x1C, 0x00, 0x1C,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        uint8_t isk2[] = {      
                        0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00,
                        
                        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                        0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        tb->m_core->io_enable = 1;

        do_test(tb, &myfile, data2, isk2, sizeof(data2));



        uint8_t data3[] = {
                        0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x45,
                        0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45,
                        0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x45,
                        0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        uint8_t isk3[] = {      
                        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        
        do_test(tb, &myfile, data3, isk3, sizeof(data3));*/

        tb->m_core->io_enable = 1;
        // Num elemnts are -> (16+8)*3 -> 60 Better 80
        uint8_t* pData = (uint8_t*) calloc (80,sizeof(uint8_t));
        uint8_t* pisk = (uint8_t*) calloc (80,sizeof(uint8_t));
        if (pData==NULL || pisk == NULL) exit (1);

        for(uint8_t first=0; first<6; first++){
                tb->m_core->io_first = first;
                for(uint8_t second=0; second<6; second++){
                        tb->m_core->io_second = second;
                        for(uint8_t third=0; third<6; third++){
                                tb->m_core->io_third = third;
                                for(uint8_t end=0; end<6; end++){
                                        tb->m_core->io_end = end;
                                        CDEFAULT
                                        printf("Do Test first = %i, second = %i, third = %i, end = %i \n", first, second, third, end);
                                        myfile << "Test State: First=" << std::to_string(first) << ", Second=" << std::to_string(second) << ", Third=" << std::to_string(third) << ", End=" << std::to_string(end) << std::endl;

                                        create_test(first, second, third, end, pData, pisk);
                                        do_test(tb, &myfile, pData, pisk, 80);
                                        
                                }
                        }
                }
        }
        


        myfile.close();

}

void create_test(uint8_t first, uint8_t second, uint8_t third, uint8_t end, uint8_t* pData, uint8_t* pisk){
        uint8_t data[] = {
                        0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x45,
                        0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45
                        
        };
        uint8_t isk[] = {      
                        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    
        };

        for(uint8_t i = 0; i<first; i++){
                pData[i] = 0;
                pisk[i] = 0;
        }

        memcpy( pData+first, data, 16 );
        memcpy( pisk+first, isk, 16 );

        for(uint8_t i = 0; i<second; i++){
                pData[i+first+16] = 0;
                pisk[i+first+16] = 0;
        }

        memcpy( pData+first+second+16, data, 16 );
        memcpy( pisk+first+second+16, isk, 16 );

        for(uint8_t i = 0; i<third; i++){
                pData[i+first+second+32] = 0;
                pisk[i+first+second+32] = 0;
        }

        memcpy( pData+first+second+third+32, data, 16 );
        memcpy( pisk+first+second+third+32, isk, 16 );

        for(uint8_t i = 0; i<end; i++){
                pData[i+first+second+third+48] = 0;
                pisk[i+first+second+third+48] = 0;
        }

}


void do_test(PCIe_RX_Scrambler_32_TB *tb, std::ofstream* myfile, uint8_t* data, uint8_t* isk, uint16_t length  ){
        // Data to be Scrambled

        uint16_t locker = 0;
        for(uint16_t i = 0; i<length; i++){
            if ( data[i] == 0xBC && isk[i] > 0 ){
                locker = i;
                break;
            }
        }

        char hex_string[3];
        *myfile << "{ ";
        for(uint16_t i = locker; i<length; i++){
                // Write Data
                sprintf(hex_string, "%02X", data[i]); 
                *myfile << std::to_string(isk[i]) << hex_string << " ";
        }
        *myfile << "}" << std::endl;
        

        uint8_t data_scrambled[length-locker] = {0};
        uint8_t isk_scrambled[length-locker] = {0};

        tb->soft_scramble(data, isk, length, myfile, data_scrambled, isk_scrambled);
        tb->enq_tx_fifo(data, isk, length);
        
        uint8_t timeout = 0;
        while(tb->rx_length() < length-locker){
                tb->sync_rx_fifo();
                /*if(tb->sync_rx_fifo()){
                        printf("Synced \n");
                }*/

                tb->deq_tx_fifo(); 
                tb->Scramblertick(); // HW Scrambler -> Do after tick ;
                tb->tick();

                timeout++;
                if(timeout > 50){
                        break;
                        printf("%i \n ", tb->rx_length());
                }
        }

        // Here unload HW Scrambler Fifo
        bool error = false;
        if ( timeout < 50){
                *myfile << "{ ";
                uint16_t f_len = length-locker; //tb->rx_length();
                for(uint16_t i = 0; i<f_len; i++){
                        // Write Data
                        sprintf(hex_string, "%02X", tb->rx_fifo.front().data); 
                        *myfile << std::to_string(tb->rx_fifo.front().isk) << hex_string << " ";
                        

                        if( tb->rx_fifo.front().data != data_scrambled[i] || tb->rx_fifo.front().isk != isk_scrambled[i] ){
                                CRED
                                if(!error){
                                        printf("Data Error @ %i dat = %02X dat_scram = %02X\n", i, tb->rx_fifo.front().data, data_scrambled[i]);
                                }
                                error = true;
                                CDEFAULT
                        }
                        //printf("Data Error @ %i dat = %02X dat_scram = %02X\n", i, tb->rx_fifo.front().data, data_scrambled[i]);
                        tb->rx_fifo.pop();
                }
                if (error){
                        CRED
                        printf("Test failed!\n");
                        CDEFAULT
                        *myfile << "} Data Error!!!" << std::endl << std::endl << std::endl;
                }else{
                        CGREEN
                        printf("Test successful\n");
                        CDEFAULT
                        *myfile << "}" << std::endl << std::endl << std::endl;
                }

                
        }
        
}