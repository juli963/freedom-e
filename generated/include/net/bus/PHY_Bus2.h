#include <stdint.h>
#include <cstddef>

#if !defined(PHY_Bus2_H)
    #define PHY_Bus2_H 1

    #if !defined(PHY_Bustype)
        #define PHY_Bustype uint8_t
    #endif
    class        PHY_Bus2{
        public:
            /*val intf = new MII_Interface()
            val Bus = new PHYBusv2(8)
            
            class PHYBusv2(DataWidth : Int) extends Bundle {
                val tx_clock = Output(Clock())
                val rx_clock = Output(Clock())
                val tx = Input(new ValidIO(new Bundle{
                    val bits = UInt(DataWidth.W)
                    val err = UInt(DataWidth.W)
                }))    // tx.valid and tx.bits
                val rx = Output(new ValidIO(new Bundle{
                    val bits = UInt(DataWidth.W)
                    val err = UInt(DataWidth.W)
                }))
                val crs = Output(Bool())
            }

            */

            struct tx_signals{          
                uint8_t* valid;
                uint8_t* strb;
                PHY_Bustype* bits;
                uint8_t* err;
            } ;      

            struct rx_signals{       
                uint8_t* valid;
                uint8_t* strb;
                PHY_Bustype* bits;
                uint8_t* err;
            } ;     

            PHY_Bus2(uint8_t* tx_clk, uint8_t* rx_clk,tx_signals txd, rx_signals rxd, uint8_t* crs_):
                tx_clock(tx_clk),
                rx_clock(rx_clk),
                tx(txd),
                rx(rxd),
                crs(crs_)
                {};
            void init();
            uint16_t get_rx_data(uint8_t* data, uint8_t* error); // return 1 when data is available
            uint16_t send_tx_data(uint8_t* data, uint8_t* error, uint16_t len);    // return is busy state
            void reset_tx_idx();
            //void get_rx_data(); 


        private:
            tx_signals tx;
            rx_signals rx;
            uint8_t* tx_clock;
            uint8_t* rx_clock;
            uint8_t* crs;
            uint8_t rx_clock_ff = 0;

            uint8_t tx_clock_ff = 0;
            uint16_t tx_idx = 0;

            const size_t datasize = sizeof(PHY_Bustype);
    };
#endif
