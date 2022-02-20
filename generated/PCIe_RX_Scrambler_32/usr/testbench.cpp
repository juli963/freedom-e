
#include "Console_Defines.h"
#include "PCIe_RX_Scrambler_32.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

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

        uint8_t data1[] = {
                        0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        uint8_t isk1[] = {      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        tb->m_core->io_enable = 1;

        do_test(tb, &myfile, data1, isk1, sizeof(data1));

        myfile.close();

}

void do_test(PCIe_RX_Scrambler_32_TB *tb, std::ofstream* myfile, uint8_t* data, uint8_t* isk, uint16_t length  ){
        // Data to be Scrambled
        char hex_string[3];
        *myfile << "{ ";
        for(uint16_t i = 0; i<length; i++){
                // Write Data
                sprintf(hex_string, "%02X", data[i]); 
                *myfile << std::to_string(isk[i]) << hex_string << " ";
        }
        *myfile << "}" << std::endl;
        

        tb->soft_scramble(data, isk, length, myfile);
        tb->enq_tx_fifo(data, isk, length);
        
        uint8_t timeout = 0;
        while(tb->rx_length() < length){
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
        *myfile << "{ ";
        uint16_t f_len = tb->rx_length();
        for(uint16_t i = 0; i<f_len; i++){
                // Write Data
                sprintf(hex_string, "%02X", tb->rx_fifo.front().data); 
                *myfile << std::to_string(tb->rx_fifo.front().isk) << hex_string << " ";
                tb->rx_fifo.pop();
        }
        *myfile << "}" << std::endl << std::endl << std::endl;
}