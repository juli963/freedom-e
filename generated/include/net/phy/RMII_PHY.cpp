#include "RMII_PHY.h"
#include <cstdio>

void RMII_PHY::init(){
    
}

uint16_t RMII_PHY::get_tx_data(uint8_t* data, bool* error, bool* nibbleend){     // return 1 when data is available
    bool ret = false;
    if(*txd.TXCLK != tx_clock_ff && *txd.TXCLK > 0){    // Rising Edge TXClk
        if( *txd.TXEN > 0 ){    // Data Valid
            if(oversampling_counter_tx == 0 ){
                *nibbleend = false;
                //*rxd.CRS = true;
                if(tx_it == 0){
                    tx_buffer = *txd.TXD;
                    tx_it++;
                }else if(tx_it == 1){
                    tx_buffer |= *txd.TXD<<2;
                    tx_it++;
                }else if(tx_it == 2){
                    tx_buffer |= *txd.TXD<<4;
                    tx_it++;
                }else{
                    tx_buffer |= *txd.TXD<<6;
                    tx_it = 0;
                    *data = tx_buffer;
                    ret = true;
                    //printf("tx_buffer: 0x%02X \n", tx_buffer);
                    //if(*txd.TXER > 0){
                    //    *error = true;
                    //}
                }
            }
            if(*speed == 0 && oversampling_counter_tx < 9){
                oversampling_counter_tx++;
            }else{
                oversampling_counter_tx = 0;
            }
        }else{
            oversampling_counter_tx = 0;
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
        uint8_t* COL;
    } ;      

*/

uint16_t RMII_PHY::send_rx_data(uint8_t* data, bool* error, bool* col, bool crs, uint16_t len){    // return is busy state

    if(*rxd.RXCLK != rx_clock_ff && *rxd.RXCLK == 0){    // Falling Edge TXClk
        if(rx_idx < len){
            if(oversampling_counter_tx == 0 ){
                //*rxd.CRS = true;
                if(rx_it == 0){
                    *rxd.RXDV = 0x01;
                    *rxd.RXD = data[rx_idx] & 0x03;
                    *rxd.RXER = error[rx_idx];
                    *rxd.COL = col[rx_idx];
                    rx_it++;
                }else if(rx_it == 1){
                    *rxd.RXDV = 0;
                    if(crs){
                        *rxd.RXDV = 0x01;
                    }
                    
                    *rxd.RXD = (data[rx_idx] & 0x0C) >> 2;
                    *rxd.RXER = error[rx_idx];
                    *rxd.COL = col[rx_idx];

                    rx_it++;
                }else if(rx_it == 2){
                    *rxd.RXDV = 0x01;
                    *rxd.RXD = (data[rx_idx] & 0x30) >> 4;
                    *rxd.RXER = error[rx_idx];
                    *rxd.COL = col[rx_idx];

                    rx_it++;
                }else{
                    *rxd.RXDV = 0;
                    if(crs){
                        *rxd.RXDV = 0x01;
                    }
                    *rxd.RXD = (data[rx_idx] & 0xC0) >> 6;
                    *rxd.RXER = error[rx_idx];
                    *rxd.COL = col[rx_idx];

                    rx_it = 0;
                    rx_idx++;
                    
                }
            }

        }else{
            if(oversampling_counter_tx == 0 ){
                *rxd.RXDV = 0;
                oversampling_counter_tx = 0;
                rx_it = 0;
                rx_idx++;
                /*if(rx_it > 0){
                    oversampling_counter_tx = 0;
                    rx_it = 0;
                    *data = tx_buffer;
                }*/
            }
        }
        if(*speed == 0 && oversampling_counter_tx < 9){
            oversampling_counter_tx++;
        }else{
            oversampling_counter_tx = 0;
        }
        /*if(rx_idx < len){
            if(rx_it == 0){
                *rxd.RXD = data[rx_idx] & 0x0F;
                rx_it++;
            }else{
                *rxd.RXD = (data[rx_idx] & 0xF0) >> 4;
                rx_it = 0;
                rx_idx++;
            }
            *rxd.COL = col[rx_idx];
            *rxd.RXER = error[rx_idx];
            *rxd.RXDV = 1;       
            rx_idx++;
        }else{
            rx_idx++;
            *rxd.RXDV = 0;     
            *rxd.RXER = 0;     
        }*/
        
        
    }
    
    rx_clock_ff = *rxd.RXCLK;
    
    return rx_idx <= len;
}

void RMII_PHY::reset_rx_idx(){
    rx_idx = 0;
    rx_it = 0;
}