#include "SGMII_PHY.h"
#include <cstdio>
#include "Console_Defines.h"

void SGMII_PHY::init(){
    
}

uint16_t SGMII_PHY::get_tx_data(uint8_t* data, bool* error, bool* nibbleend){     // return 1 when data is available
    uint16_t idx = 0;
    if(*txd.TXCLOCK != tx_clock_ff && *txd.TXCLOCK > 0){    // Rising Edge TXClk
        
        for(uint8_t i = 0; i<datasize; i++){
            if(((uint8_t*)txd.TXDATA)[i] == dEOF && (*txd.TXDATAK & 1<<i) > 0){ // EOF
                tx_lock = false;
            }
            if(tx_lock){
                if(((uint8_t*)txd.TXDATA)[i] == dSOF && (*txd.TXDATAK & 1<<i) > 0){ // SOF in SOF
                    CRED
                    printf("ERROR SOF following SOF \n");
                    CDEFAULT
                    error[idx] = true;
                }
                data[idx] = ((uint8_t*)txd.TXDATA)[i];
                if(((uint8_t*)txd.TXDATA)[i] == dERROR_FR && (*txd.TXDATAK & 1<<i) > 0){ // SOF
                    error[idx] = 1;
                }
                nibbleend[idx] = 0;
                idx++;
            }
            if(((uint8_t*)txd.TXDATA)[i] == dSOF && (*txd.TXDATAK & (1<<i)) > 0){ // SOF
                tx_lock = true;
            }
        }
    } 
    
    tx_clock_ff = *txd.TXCLOCK;
    return idx;
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

/* ToDo: Send SOF in SOF and see what happens */

uint16_t SGMII_PHY::send_rx_data(uint8_t* data, bool* error, bool* col, uint16_t len){    // return is busy state
    if(*rxd.RXCLOCK != rx_clock_ff && *rxd.RXCLOCK == 0){    // Falling Edge TXClk
        *rxd.RXDATA = 0;
        *rxd.RXDATAK = 0;
        for(uint8_t i = 0; i<datasize; i++){

            if(rx_idx < len){
                if((rx_idx-1) > 0 ){      // We can look one Byte before
                    if(col[rx_idx-1] == true && col[rx_idx] == false){ // Let's look if we find SOF -> Falling edge on col (only for SGMII)
                        *rxd.RXDATA |= (uint16_t)dEOF<<(8*i);
                        *rxd.RXDATAK |= 1<<i;
                        rx_lock = false;
                    }
                }   

                if(rx_lock){
                                //printf("IT %i\n", i);
                    *rxd.RXDATA |= (uint16_t)data[rx_idx]<<(8*i);
                    if(error[rx_idx]){
                        *rxd.RXDATA |= (uint16_t)dERROR_FR<<(8*i);
                        *rxd.RXDATAK |= 1<<i;
                    }
                }

                if((rx_idx+1) < len ){      // We can look one Byte ahead
                    if(col[rx_idx] == false && col[rx_idx+1] == true){ // Let's look if we find SOF -> Rising edge on col (only for SGMII)
                        *rxd.RXDATA |= (uint16_t)dSOF<<(8*i);
                        *rxd.RXDATAK |= 1<<i;
                        rx_lock = true;
                    }
                }   

                rx_idx++;
            }else{
                if(rx_lock == true){
                    *rxd.RXDATA |= (uint16_t)dEOF<<(8*i);
                    *rxd.RXDATAK |= 1<<i;
                    rx_lock = false;
                }
                rx_idx++;
            }
        }
        //printf("0x%04X 0x%04X \n", *rxd.RXDATA, *rxd.RXDATAK);
    }
    
    rx_clock_ff = *rxd.RXCLOCK;
    
    return rx_idx <= len;
}

void SGMII_PHY::reset_rx_idx(){
    rx_idx = 0;
}