#include <stdint.h>

#if !defined(RMII_PHY_H)
    #define RMII_PHY_H 1
    class        RMII_PHY{
        public:
            /*val intf = new MII_Interface()
            val Bus = new PHYBusv2(8)
            
            class MII_Interface() extends Bundle { // 25/2.5 MHz
                val tx = new Bundle{
                    val TXCLK = Input(Clock())  
                    val TXEN = Output(Bool())   
                    val TXER = Output(Bool())   
                    val TXD = Output(UInt(4.W))
                }
                val rx = new Bundle{
                    val RXCLK = Input(Clock())
                    val RXDV = Input(Bool())
                    val RXER = Input(Bool())
                    val CRS = Input(Bool())
                    val COL = Input(Bool())
                    val RXD = Input(UInt(4.W))
                }
            }
            */

            struct tx_signals{          
                uint8_t* TXCLK;
                uint8_t* TXEN;
                uint8_t* TXD;
            } ;      

            struct rx_signals{       
                uint8_t* RXCLK;
                uint8_t* RXDV;
                uint8_t* RXER; 
                uint8_t* RXD;
                uint8_t* COL;
            } ;      


            RMII_PHY(tx_signals tx, rx_signals rx, uint8_t* speed_):
                txd(tx),
                rxd(rx),
                speed(speed_)
                {};
            void init();
            bool get_tx_data(uint8_t* data, bool* error, bool* nibbleend); // return 1 when data is available
            bool send_rx_data(uint8_t* data, bool* error, bool* col, bool crs, uint16_t len);    // return is busy state
            void reset_rx_idx();
            //void send_rx_data();


        private:
            tx_signals txd;
            rx_signals rxd;
            uint8_t tx_clock_ff = 0;
            uint8_t tx_buffer = 0;
            uint8_t tx_it = 0;  // iteration
            uint8_t oversampling_counter_tx = 0;  // iteration
            uint8_t* speed;  // iteration

            uint16_t rx_idx = 0;
            uint8_t rx_it = 0;  // iteration
            uint8_t rx_clock_ff = 0;
    };
#endif
