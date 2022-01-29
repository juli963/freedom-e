#if !defined(Ethernet_H)
    #include "testbench.h"
    #include "stdint.h"
    #include "c8b10b_to_udp_test.h"
    #include <queue>

    #define Ethernet_H 1
    class Ethernet {
        
        public:
            Ethernet(c8b10b_to_udp_test_TB *tb) { // Constructor with parameters
                this->tb = tb;
            }
            c8b10b_to_udp_test_TB *tb;
            void init();
            void tick();
            void fill_tx_fifo(uint8_t* data, uint16_t length);    // Prints Input Datastream
            void clear_rx_fifo();   // Prints Output Datastream

            std::queue <uint8_t> tx_data;
            std::queue <uint16_t> tx_length;
            uint16_t tx_act_length = 0;
            uint8_t tx_gap = 0;

            std::queue <uint8_t> rx_data;
            std::queue <uint16_t> rx_length;
            bool in_rx = false;
            uint8_t last_strb = 0;
            uint16_t rx_act_length = 0;

            bool print_output = true;

            uint8_t srcMAC[6] = {
                0x09,0x08,0x07,0x06,0x05,0x04
            };
            uint8_t srcIP[4] = {
                0xC0,0xA8,0x07,0x06
            };

            struct s8b10b {   
                uint8_t data;
                uint8_t isk;
            };
            
            std::queue <s8b10b> fifo_data;
            std::queue <uint16_t> fifo_mgmt;

            void createEthernetFrame(uint8_t* destMAC, uint16_t etherType, uint8_t* data, uint16_t length);
            void createARPFrame(uint8_t* searchIP);
            void createIP4Frame(uint8_t* data, uint16_t length, uint8_t* destIP, uint8_t* destMAC, uint8_t protocol);
            void createUDPFrame(uint8_t* data, uint16_t length, uint8_t* destIP, uint8_t* destMAC, uint16_t srcPort, uint16_t destPort);
            void createStreamFrame(uint8_t* destIP, uint8_t* destMAC, uint8_t type, uint32_t flags, uint64_t rsvd, uint32_t* data, uint16_t data_len);

        private:
            void printEthernet(uint8_t* data, uint16_t length);
            void printIP(uint8_t* data, uint16_t length);
            void printUDP(uint8_t* data, uint16_t length, uint8_t* pseudo_header);
            void printTCP(uint8_t* data, uint16_t length);
            void printARP(uint8_t* data, uint16_t length);
            void printESP(uint8_t* data, uint16_t length);
            bool checkIPChecksum(uint8_t* data, uint16_t length, uint16_t checksum);
            uint16_t calcIPChecksum(uint8_t* data, uint16_t length);
    };
#endif
