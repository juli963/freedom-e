#include <stdint.h>
#include "PHY.h"

#if !defined(SGMII_PHY_H)
    #define SGMII_PHY_H 1

    /*#if !defined(PHY_Bustype)
        #define PHY_Bustype uint8_t
    #endif*/

    #if Datawidth == 8
            #define PHY_Bustype uint8_t
    #elif Datawidth == 16
            #define PHY_Bustype uint16_t
    #elif Datawidth == 32
            #define PHY_Bustype uint32_t
    #elif Datawidth == 64 
            #define PHY_Bustype uint64_t
    #endif

    class        SGMII_PHY  : public virtual PHY{
        public:
            virtual ~SGMII_PHY() {};

            struct tx_signals{          
                uint8_t* TXCLOCK;
                PHY_Bustype* TXDATA;
                uint8_t* TXDATAK;
            } ;      

            struct rx_signals{       
                uint8_t* RXCLOCK;
                PHY_Bustype* RXDATA;
                uint8_t* RXDATAK;
            } ;      


            SGMII_PHY(tx_signals tx, rx_signals rx):
                txd(tx),
                rxd(rx)
                {};
            void init();
            uint16_t get_tx_data(uint8_t* data, bool* error, bool* nibbleend); // return 1 when data is available
            uint16_t send_rx_data(uint8_t* data, bool* error, bool* col, uint16_t len);    // return is busy state
            void reset_rx_idx();
            //void send_rx_data();


        private:
            tx_signals txd;
            rx_signals rxd;
            uint8_t tx_clock_ff = 0;
            uint8_t tx_buffer = 0;
            bool tx_lock = false;  // locked on receiving

            uint16_t rx_idx = 0;
            uint8_t rx_it = 0;  // iteration
            uint8_t rx_clock_ff = 0;
            bool rx_lock = false;

            const uint8_t dEOF = 0xFD;
            const uint8_t dSOF = 0xFB;
            const uint8_t dERROR_FR = 0xFE;

            const size_t datasize = sizeof(PHY_Bustype);
    };
#endif
