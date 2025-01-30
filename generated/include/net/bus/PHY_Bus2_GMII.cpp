#include "PHY_Bus2.h"
#include <cstdio>
void PHY_Bus2::init(){

}

uint16_t PHY_Bus2::get_rx_data(uint8_t* data, uint8_t* error){ // return 1 when data is available(){

    bool ret;
    if(*rx_clock != rx_clock_ff && *rx_clock > 0){    // Rising Edge TXClk
        if( *rx.valid > 0 && *rx.strb > 0){
            *data = *rx.bits;
            if(*rx.err > 0){
                *error = true;
            }else{
                *error = false;
            }
            ret = true;
        }else{
            ret = false;
        }
    }else{
        ret = false;
    }
    
    rx_clock_ff = *rx_clock;
    return ret;
}

uint16_t PHY_Bus2::send_tx_data(uint8_t* data, uint8_t* error, uint16_t len){    // return is busy state

    if(*tx_clock != tx_clock_ff && *tx_clock == 0){    // Falling Edge TXClk
        if(tx_idx < len){
            *tx.bits = data[tx_idx];
            *tx.err = error[tx_idx];
            *tx.valid = true;     
            *tx.strb = 1;     
            tx_idx++;
        }else{
            tx_idx++;
            *tx.valid = false;     
            *tx.strb = false;     
        }
        
        
    }
    
    tx_clock_ff = *tx_clock;
    
    return tx_idx <= len;

}

void PHY_Bus2::reset_tx_idx(){
    tx_idx = 0;
}