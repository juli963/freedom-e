#include <stdint.h>
#include "PHY.h"

#if !defined(GMII_PHY_H)
    #define GMII_PHY_H 1
    class        GMII_PHY : public virtual PHY{
        public:
            /*
            val tx = new Bundle{
                val  GTXCLOCK = Output(Clock())
                val  TXCLK = Input(Clock())    // Used for 10/100
                val  TXEN = Output(Bool())   
                val  TXER = Output(Bool())   
                val  TXD = Output(UInt(8.W))
            }
            val rx = new Bundle{
                val  RXCLK = Input(Clock())
                val  RXDV = Input(Bool())
                val  RXER = Input(Bool())
                val  COL = Input(Bool())
                val  CRS = Input(Bool())
                val  RXD = Input(UInt(8.W))
            */

            struct tx_signals{          
                uint8_t* GTXCLOCK;
                uint8_t* TXEN;
                uint8_t* TXER; 
                uint8_t* TXD;
            } ;      

            struct rx_signals{       
                uint8_t* RXCLK;
                uint8_t* RXDV;
                uint8_t* RXER; 
                uint8_t* RXD;
                uint8_t* CRS;
                uint8_t* COL;
            } ;      


            GMII_PHY(tx_signals tx, rx_signals rx):
                txd(tx),
                rxd(rx)
                {};
            void init();
            bool get_tx_data(uint8_t* data, bool* error, bool* nibbleend); // return 1 when data is available
            bool send_rx_data(uint8_t* data, bool* error, bool* col, uint16_t len);    // return is busy state
            void reset_rx_idx();
            //void send_rx_data();


        private:
            tx_signals txd;
            rx_signals rxd;
            uint8_t tx_clock_ff = 0;
            uint8_t tx_buffer = 0;
            uint8_t tx_it = 0;  // iteration

            uint16_t rx_idx = 0;
            uint8_t rx_it = 0;  // iteration
            uint8_t rx_clock_ff = 0;
    };
#endif
