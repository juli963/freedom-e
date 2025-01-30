#include "GMII_PHY.h"
#include <cstdio>

void GMII_PHY::init(){
    
}

uint16_t GMII_PHY::get_tx_data(uint8_t* data, bool* error, bool* nibbleend){     // return 1 when data is available
    bool ret = false;
    if(*txd.GTXCLOCK != tx_clock_ff && *txd.GTXCLOCK > 0){    // Rising Edge TXClk
        if( *txd.TXEN > 0 ){    // Data Valid
            *nibbleend = false;
            if(*txd.TXER > 0){
                *error = true;
            }
            *data = *txd.TXD;
            ret = true;

            //*rxd.CRS = true;
            /*if(tx_it == 0){
                tx_buffer = *txd.TXD;
                tx_it++;
                if(*txd.TXER > 0){
                    *error = true;
                }
            }else{
                tx_buffer |= *txd.TXD<<4;
                tx_it = 0;
                *data = tx_buffer;
                ret = true;
                //printf("tx_buffer: 0x%02X \n", tx_buffer);
                if(*txd.TXER > 0){
                    *error = true;
                }
            }*/
       // }else{
            //*rxd.CRS = false;
            //if(tx_it > 0){
            //    tx_it = 0;
             //   *nibbleend = true;
            //    *data = tx_buffer;
            //    ret = true;
            //}
        }
    } 
    
    tx_clock_ff = *txd.GTXCLOCK;
    return ret;
}; 

/*

    struct rx_signals{       
        uint8_t* RXCLK;
        uint8_t* RXDV;
        uint8_t* RXER; 
        uint8_t* RXD;
        uint8_t* CRS;
        uint8_t* COL;
    } ;      

*/

uint16_t GMII_PHY::send_rx_data(uint8_t* data, bool* error, bool* col, uint16_t len){    // return is busy state

    if(*rxd.RXCLK != rx_clock_ff && *rxd.RXCLK == 0){    // Falling Edge TXClk
        if(rx_idx < len){
            /*if(rx_it == 0){
                *rxd.RXD = data[rx_idx] & 0x0F;
                rx_it++;
            }else{
                *rxd.RXD = (data[rx_idx] & 0xF0) >> 4;
                rx_it = 0;
                rx_idx++;
            }*/

            *rxd.RXD = data[rx_idx];
            *rxd.COL = col[rx_idx];
            *rxd.RXER = error[rx_idx];
            *rxd.RXDV = 1;       
            //*rxd.CRS = true;
            rx_idx++;
        }else{
            //*rxd.CRS = false;
            rx_idx++;
            *rxd.RXDV = 0;     
            *rxd.RXER = 0;     
        }
        
        
    }
    
    rx_clock_ff = *rxd.RXCLK;
    
    return rx_idx <= len;
}

void GMII_PHY::reset_rx_idx(){
    rx_idx = 0;
}