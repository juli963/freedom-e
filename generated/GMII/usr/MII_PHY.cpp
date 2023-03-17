#include "MII_PHY.h"
#include <cstdio>

void MII_PHY::init(){
    
}

bool MII_PHY::get_tx_data(uint8_t* data, bool* error, bool* nibbleend){     // return 1 when data is available
    bool ret = false;
    if(*txd.TXCLK != tx_clock_ff && *txd.TXCLK > 0){    // Rising Edge TXClk
        if( *txd.TXEN > 0 ){    // Data Valid
            *nibbleend = false;
            //*rxd.CRS = true;
            if(tx_it == 0){
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
            }
        }else{
            //*rxd.CRS = false;
            if(tx_it > 0){
                tx_it = 0;
                *nibbleend = true;
                *data = tx_buffer;
                ret = true;
            }
        }
    } 
    
    tx_clock_ff = *txd.TXCLK;
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

bool MII_PHY::send_rx_data(uint8_t* data, bool* error, bool* col, uint16_t len){    // return is busy state

    if(*rxd.RXCLK != rx_clock_ff && *rxd.RXCLK == 0){    // Falling Edge TXClk
        if(rx_idx < len){
            *rxd.COL = col[rx_idx];
            *rxd.RXER = error[rx_idx];
            *rxd.RXDV = 1;  
            if(rx_it == 0){
                *rxd.RXD = data[rx_idx] & 0x0F;
                rx_it++;
            }else{
                *rxd.RXD = (data[rx_idx] & 0xF0) >> 4;
                rx_it = 0;
                rx_idx++;
            }
     
            //*rxd.CRS = true;
            //rx_idx++;
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

void MII_PHY::reset_rx_idx(){
    rx_idx = 0;
}