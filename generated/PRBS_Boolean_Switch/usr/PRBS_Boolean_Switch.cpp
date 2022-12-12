
#include "PRBS_Boolean_Switch.h"
#include <bit>
#include <bitset>
#include <cstdint>


void PRBS_Boolean_Switch_TB::init(){
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}

void PRBS_Boolean_Switch_TB::init_polynom(uint8_t* arr, uint8_t len, uint8_t width){
    for(uint8_t i = 0; i< width;i++){
        lfsr_mask |= ((uint64_t)1<<i);
    }
    lfsr_width = width;
    for(uint8_t i = 0; i< len;i++){
        PRBS_polynom |= (1<<(arr[i]-1));
    }
}

uint64_t PRBS_Boolean_Switch_TB::get_next_lfsr(uint64_t input){
	uint64_t init = input;
    uint64_t poly = PRBS_polynom;
    uint8_t bit = 0;
    for(uint8_t i = 0; i<32; i++){
        if ((poly & (1<<0)) > 0){
            bit ^= init;
        }
        init >>= 1;
        poly >>= 1;
    }
    bit &= 0x01;
    return ((input << 1) | bit) & lfsr_mask;
}

uint8_t PRBS_Boolean_Switch_TB::calculate_bool(uint64_t seed){
    //out := PopCount(lfsr.io.out) > (width/2).toInt.U //lfsr.io.out.xorR //lfsr.io.out > (Math.pow(2,width)/2).ceil.toInt.U
    std::bitset<64> bs( seed );

    if(bs.count() > lfsr_width/2){
        return 1;
    }else{
        return 0;
    }

}
