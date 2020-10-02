#include "LFSR.h"

void LFSR_TB::init(){
    m_core->io_increment = 0;
    reset();
}

void LFSR_TB::shift_HW(){
    m_core->io_increment = 1;
    tick();
    m_core->io_increment = 0;
}

uint64_t LFSR_TB::taps_to_polynom(const uint8_t* taps, uint8_t num_taps){
    uint64_t ret = 0;
    for(uint8_t i = 0; i<num_taps;i++){
        ret |= (1<<(taps[i]-1));
    }
    return ret;
}

void LFSR_TB::shift_SW(uint64_t polynom, uint64_t mask, uint64_t* var){
    uint64_t init = *var;
    uint64_t poly = polynom;
    uint8_t bit = 0;
    for(uint8_t i = 0; i<32; i++){
        if ((poly & (1<<0)) > 0){
            bit ^= init;
        }
        init >>= 1;
        poly >>= 1;
    }
    bit &= 0x01;
    *var = ((*var << 1) | bit) & mask;
}

uint64_t LFSR_TB::get_HW(){
    return (uint64_t)m_core->io_out;
}