#include "RGMII_PHY.h"
#include <cstdio>

void RGMII_PHY::init(){
    
}

uint16_t RGMII_PHY::get_tx_data(uint8_t* data, bool* error, bool* nibbleend){     // return 1 when data is available
    bool ret = false;
    if(*txd.GTXCLOCK != tx_clock_ff && *txd.GTXCLOCK > 0){    // Rising Edge TXClk
        if( *txd.TX_CTL_rise > 0 ){    // Data Valid
            *nibbleend = false;
            if(*txd.TX_CTL_fall == 0){
                *error = true;
            }
            *data = *txd.TXD_rise | ((*txd.TXD_fall)<<4);
            ret = true;
        }
    } 
    
    tx_clock_ff = *txd.GTXCLOCK;
    return ret;
}; 

uint16_t RGMII_PHY::send_rx_data(uint8_t* data, bool* error, bool* col, uint16_t len){    // return is busy state

    if(*rxd.RXC != rx_clock_ff && *rxd.RXC == 0){    // Falling Edge TXClk
        if(rx_idx < len){

            *rxd.RXD_rise = data[rx_idx] & 0x0F;
            *rxd.RXD_fall = (data[rx_idx] & 0xF0)>>4;
            if(error[rx_idx]){
                *rxd.RXCTL_fall = 0;
            }else{
                *rxd.RXCTL_fall = 1;
            }
            *rxd.RXCTL_rise = 1;       
            rx_idx++;
        }else{
            rx_idx++;
            *rxd.RXCTL_rise = 0;     
            *rxd.RXCTL_fall = 0;   
        }
        
        
    }
    
    rx_clock_ff = *rxd.RXC;
    
    return rx_idx <= len;
}

void RGMII_PHY::reset_rx_idx(){
    rx_idx = 0;
}