
#include "testbench.h"
#include "../obj_dir/VTLP_w_Configuration.h"
#include "stdint.h"
#include <queue>

#if !defined(TLP_w_Configuration_H)
    #define TLP_w_Configuration_H 1
    class TLP_w_Configuration_TB : public TESTBENCH<VTLP_w_Configuration> {
        public:
            uint8_t id;
            std::queue<uint8_t> qdata_rx;
            std::queue<uint8_t> qstrb_rx;
            std::queue<uint8_t> qdata;
            std::queue<uint8_t> qstrb;
            bool in_frame = false;  // Is in Frame before FIFO
            bool frame_active = false; // Is in Frame behind FIFO
            bool ack = false;
            uint16_t seq_number = 0;
            TLP_w_Configuration_TB(uint8_t x) { 
                id = x;
            }
            void init();
            void queue(TLP_w_Configuration_TB* otb);
            void fill_rx(uint16_t isk, uint16_t data);
            void Create_Configuration_TLP(bool is_read ,bool TD, bool EP, uint16_t Requester, uint8_t Tag, uint16_t Completer, uint16_t Register, uint8_t first_DW_BE, uint32_t data);
            void Create_ACK_DLLP(bool is_ACK, uint16_t seq_number);
            void Print_Completion_TLP();
            uint32_t CalculateTlpCRC(uint8_t* data, uint8_t len);
            uint16_t CalculateDllpCRC(uint8_t type, uint8_t* data, uint8_t len);
        private:


    };
#endif
