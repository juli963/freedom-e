
#include "testbench.h"
//#include "../obj_dir/VSniffer_8b10b_40.h"
#include "../obj_dir/Vc8b10b_to_udp_test.h"
#include "stdint.h"
#include <queue>

#if !defined(Sniffer_8b10b_40_H)
    #define Sniffer_8b10b_40_H 1
    class        Sniffer_8b10b_40_TB {
        public:

            struct s8b10b {   
                uint8_t data;
                uint8_t isk;
            };

            enum eTriggerMode          
            {
                edisable = 0,     
                estart_stop,     
                elength     
            };

            Vc8b10b_to_udp_test        *m_core;

            Sniffer_8b10b_40_TB(Vc8b10b_to_udp_test        *m_core){
                this->m_core = m_core;
            };

            void init();
            void deq_rx_fifo();
            void enq_rx_fifo(uint8_t* data, uint8_t* isk, uint16_t length);

            void enable_trigger_length(s8b10b start, uint16_t length, eTriggerMode mode, uint8_t num_trigger);
            void enable_trigger_code(s8b10b start, s8b10b stop, eTriggerMode mode, uint8_t num_trigger);
            void sel_gtp(uint8_t num);

            std::queue <s8b10b> rx_fifo;
        private:
            void set_8b10b(s8b10b* p8b10b);
    };
#endif
