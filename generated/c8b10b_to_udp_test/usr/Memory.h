#if !defined(Memory_H)
    #include "testbench.h"
    #include "stdint.h"
    #include "c8b10b_to_udp_test.h"
    #include <queue>

    #define Memory_H 1
    
    #define Memory_Native 1
    #define Memory_FIFO 1

    class Memory {
        
        public:
            Memory(c8b10b_to_udp_test_TB *tb, uint32_t depth) { // Constructor with parameters
                this->tb = tb;
                this->depth = depth;

            }
            c8b10b_to_udp_test_TB *tb;
            uint32_t depth;
            bool deq_en = false;
            bool triggered = false;
            void init();
            void tick();
            void add_data(uint8_t type, uint64_t flags, uint16_t length, uint8_t* data, uint64_t timestamp_start, uint64_t timestamp_end);

            std::queue <uint64_t> tx_fifo_data;            

        private:
            void tick_native();
            void tick_fifo();
            uint32_t idx;
            uint8_t fifo_ready_ff = 0;
            std::vector<uint64_t> mem_data = std::vector<uint64_t>(depth);
    };
#endif
