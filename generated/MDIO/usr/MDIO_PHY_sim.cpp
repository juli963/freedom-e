#include "MDIO_PHY_sim.h"
#include "Console_Defines.h"
#include <stdio.h>

void MDIO_PHY_sim::main(){

    convert_mdio_tri();
    // On Falling Edge Data is Valid
    if(clock_falling()){
        switch(state){      
            case e_idle: 
                {
                    if(temp == 0xFFFFFFFF){ // Preamble is there -> Look for start Bit
                        if(mdio_tri_con == 0){ // Start Bit
                            state = e_start;
                        }
                    }
                    temp <<= 1;
                    temp |= mdio_tri_con&0x01;
                }
                break;
            case e_start: 
                {
                    #ifdef MDIO_PHY_DEBUG
                        printf("Start Signal arrived! \n");
                    #endif
                    if( mdio_tri_con != 0 ){
                        state = e_opcode;

                        phyaddress = 0;
                        regaddress = 0;
                        opcode = 0;
                        data = 0;
                        bitcounter = 0;
                    }
                }
                break;
            case e_opcode:
                {
                    opcode <<= 1;
                    opcode |= mdio_tri_con&0x01;
                    bitcounter++;

                    if(bitcounter >= 2){
                        state = e_phyadress;
                        bitcounter = 0;
                        #ifdef MDIO_PHY_DEBUG
                            printf("Opcode#: %i \n", opcode);
                            printf("Op is write: %i \n", is_write());
                        #endif
                    }
                }
                break;
            case e_phyadress: 
                {
                    phyaddress <<= 1;
                    phyaddress |= mdio_tri_con&0x01;
                    
                    bitcounter++;

                    if(bitcounter >= 5){
                        state = e_regadress;
                        bitcounter = 0;
                        #ifdef MDIO_PHY_DEBUG
                            printf("Phy#: %i \n", phyaddress);
                        #endif
                        if(phyaddress > 31){
                            #ifdef MDIO_PHY_DEBUG
                                CRED
                                printf("Error: Phyaddress is too high!\n");
                                CDEFAULT
                            #endif
                            set_error();
                        }
                    }
                }
                break;
            case e_regadress: 
                {
                    regaddress <<= 1;
                    regaddress |= mdio_tri_con&0x01;
                    bitcounter++;

                    if(bitcounter >= 5){
                        state = e_turnaround;
                        bitcounter = 0;
                        #ifdef MDIO_PHY_DEBUG
                            printf("Reg#: %i \n", regaddress);
                        #endif
                        if(regaddress > 31){
                            #ifdef MDIO_PHY_DEBUG
                                CRED
                                printf("Error: Regadress is too high!\n");
                                CDEFAULT
                            #endif
                            set_error();
                        }
                    }
                }
                break;
            case e_turnaround: 
                {
                    if(bitcounter == 0 && mdio_tri_con == 0){
                        #ifdef MDIO_PHY_DEBUG
                            CRED
                            printf("Error: @Turnaround not 1\n");
                            CDEFAULT
                        #endif
                        set_error();
                    }
                    if(bitcounter == 1 && is_write() && mdio_tri_con != 0){
                        #ifdef MDIO_PHY_DEBUG
                            CRED
                            printf("Error: @Turnaround and Write not 0\n");
                            CDEFAULT
                        #endif
                        set_error();
                    }

                    bitcounter++;

                    if(bitcounter >= 2){
                        #ifdef MDIO_PHY_DEBUG
                            printf("TA Complete! \n");
                        #endif
                        state = e_data;
                        bitcounter = 0;
                        if(regaddress <= 31 && !is_write()){
                            data = registers[regaddress];  
                        }
                    }
                }
                break;
            case e_data: 
                {
                    if( is_write() ){
                        data <<= 1;
                        data |= mdio_tri_con&0x01;
                        bitcounter++;

                        if(bitcounter >= 16){
                            if(regaddress <= 31 && is_write() && is_phy()){
                                registers[regaddress] = data;
                            }
                            state = e_end;
                            bitcounter = 0;
                            #ifdef MDIO_PHY_DEBUG
                                printf("Data#: 0x%04X \n", data);
                            #endif
                        }
                    }
                }
                break;
            case e_end: 
                {
                    bitcounter++;
                    if(bitcounter > 1){
                        *mdio_in = 1;
                        state = e_idle;
                    }
                }
                break;
            default: 
                state = e_idle;
                break;
        }
    }

    if(clock_rising()){
        
        switch(state){   

            case e_turnaround: 
                {
                    if(bitcounter == 1 && !is_write() && is_phy()){
                        *mdio_in = 0;
                    }
                }
                break;

            case e_data: 
                {   
                    if( !is_write() && is_phy() ){
                        *mdio_in = (data & (0x01<<(15-bitcounter))) >>(15-bitcounter);
                        bitcounter++;

                        if(bitcounter >= 16){
                            bitcounter = 0;
                            state = e_end;
                        }
                    }
                }
                break;
            default: 
                break;
        }

    }
    

    mclk_ff = *mclk;
}

void MDIO_PHY_sim::print_registers(){
    for(uint8_t i = 0; i<32; i++){
        printf("Reg #%i: 0x%04X\n", i, registers[i]);
    }
}

void MDIO_PHY_sim::init(){
    state = e_idle;
    *mdio_in = 1;
    mclk_ff = *mclk;
    temp = 0;
}

void MDIO_PHY_sim::convert_mdio_tri(){
    mdio_tri_con = *mdio_tri;
    if(mdio_tri_con != 0){
        //*mdio_tri = 1;
        mdio_tri_con = 1;
    }
}

bool MDIO_PHY_sim::clock_rising(){
    if( *mclk != 0 && mclk_ff == 0 ){
        return true;
    }else{
        return false;
    }
}

bool MDIO_PHY_sim::clock_falling(){
    if( *mclk == 0 && mclk_ff != 0 ){
        return true;
    }else{
        return false;
    }
}

bool MDIO_PHY_sim::is_write(){
    if( (opcode&0x02) == 0){
        return true;
    }else{
        return false;
    }
}

bool MDIO_PHY_sim::is_phy(){
    return const_phyad == phyaddress;
}

void MDIO_PHY_sim::set_error(){
    *berror = true;
}

