
#include "testbench.h"
#include "../obj_dir/VPCIe_RX_Scrambler_32.h"
#include "stdint.h"
#include <queue>
#include <fstream>

#if !defined(PCIe_RX_Scrambler_32_H)
    #define PCIe_RX_Scrambler_32_H 1
    class        PCIe_RX_Scrambler_32_TB : public TESTBENCH<VPCIe_RX_Scrambler_32> {
        public:
            struct s8b10b {   
                uint8_t data;
                uint8_t isk;
            };

            uint16_t scrambler_val = 0xFFFF;
            uint8_t scramxor = 0xFF;
            bool change_dat = false;

            void deq_tx_fifo();
            void enq_tx_fifo(uint8_t* data, uint8_t* isk, uint16_t length);

            void init();

            /*
                return value is XOR value
                state is LFSR value
            */
            uint8_t RunScrambler();
            void Scramblertick();
            void soft_scramble(uint8_t* data, uint8_t* isk, uint16_t length, std::ofstream* myfile);
            bool sync_rx_fifo();
            uint16_t rx_length(); 
            std::queue <s8b10b> tx_fifo;
            std::queue <s8b10b> rx_fifo;
        private:
            void set_8b10b(s8b10b* p8b10b);

    };
#endif
