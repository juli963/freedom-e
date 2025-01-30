#include <stdint.h>
#include "PHY.h"

#if !defined(RGMII_PHY_H)
    #define RGMII_PHY_H 1
    class        RGMII_PHY : public virtual PHY{
        public:
            /*
                .RXC = &tb->m_core->io_intf_rx_RXC,
                .RXCTL_rise = &tb->m_core->io_intf_rx_RX_CTL_rise, 
                .RXCTL_fall = &tb->m_core->io_intf_rx_RX_CTL_fall, 
                .RXD_rise = &tb->m_core->io_intf_rx_RXD_rise,
                .RXD_fall = &tb->m_core->io_intf_rx_RXD_fall


                .GTXCLOCK = &tx_clock,
                .TX_CTL_rise = &tb->m_core->io_intf_tx_TX_CTL_rise, 
                .TX_CTL_fall = &tb->m_core->io_intf_tx_TX_CTL_fall, 
                .TXD_rise = &tb->m_core->io_intf_tx_TXD_rise,
                .TXD_fall = &tb->m_core->io_intf_tx_TXD_fall,
            */

            struct tx_signals{          
                uint8_t* GTXCLOCK;
                uint8_t* TX_CTL_rise;
                uint8_t* TX_CTL_fall;
                uint8_t* TXD_rise; 
                uint8_t* TXD_fall;
            } ;      

            struct rx_signals{       
                uint8_t* RXC;
                uint8_t* RXCTL_rise;
                uint8_t* RXCTL_fall; 
                uint8_t* RXD_rise;
                uint8_t* RXD_fall;
            } ;      


            RGMII_PHY(tx_signals tx, rx_signals rx):
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
            uint8_t tx_it = 0;  // iteration

            uint16_t rx_idx = 0;
            uint8_t rx_it = 0;  // iteration
            uint8_t rx_clock_ff = 0;
    };
#endif
