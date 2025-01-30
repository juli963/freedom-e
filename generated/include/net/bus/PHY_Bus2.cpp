#include "PHY_Bus2.h"
#include <cstdio>
void PHY_Bus2::init(){

}

uint16_t PHY_Bus2::get_rx_data(uint8_t* data, uint8_t* error){ // return 1 when data is available(){

    uint16_t ret = 0;
    uint8_t x = 0;
    if(*rx_clock != rx_clock_ff && *rx_clock > 0){    // Rising Edge RX
        for(uint8_t i = 0; i<datasize; i++){
            if( *rx.valid > 0 && ((*rx.strb)&(1<<i)) > 0){
                data[x] = ((uint8_t*)rx.bits)[i];
                if(((*rx.err)&(1<<i)) > 0){
                    error[i] = true;
                }else{
                    error[i] = false;
                }
                ret++;
                x++;
            }
        }

    }else{
        ret = 0;
    }
    
    rx_clock_ff = *rx_clock;
    return ret;
}

uint16_t PHY_Bus2::send_tx_data(uint8_t* data, uint8_t* error, uint16_t len){    // return is busy state
        //printf("Sizeof %i\n", datasize);
    if(*tx_clock != tx_clock_ff && *tx_clock == 0){    // Falling Edge TXClk  
        if(tx_idx < len){
            uint8_t x = 0;
            *tx.strb = 0;     
            *tx.err = 0;
            *tx.valid = true;
            for(uint8_t i = 0; i<datasize; i++){
                if(tx_idx < len){
                    tx.bits[i] = data[tx_idx];
                    if( error[tx_idx] ){
                        *tx.err |= 1<<i;
                    }                       
                    *tx.strb |= 1<<i;     
                    tx_idx++;
                }
            }

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