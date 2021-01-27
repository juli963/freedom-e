#include"PCIe_detectors.h"
#include"PCIe_Types.h"

void TS_Detector::Detect(uint16_t rxdata, uint8_t charisk, bool valid){
    uint8_t *rx = (uint8_t*)&rxdata;
    bool isk[2];
    uint8_t shift = 1;
    for (uint8_t i = 0; i<2; i++){
        isk[i] = (charisk&shift) > 0;
        shift = shift << 1; 
    }
    switch(state){
        case 0:
            is = 0;
            if ( (PCIe_Types::COM == rx[0]) && (isk[0]) && valid )
            {
                byteCounter = 0;
                state = state + 1;
                data.num_link = rx[1];
            }else{
                is_cons = false;
            }
            break;
        case 1:
            state = state + 1;
            data.num_lane = rx[0];
            data.n_fts = rx[1];
            break;
        case 2:
            state = state + 1;
            data.data_rate = rx[0];
            data.training_control = rx[1];
            break;
        case 3: // Check if TS1 or TS2
            if ( byteCounter >= 8 || 
                (rx[0] != PCIe_Types::TS1_Ident && rx[0] != PCIe_Types::TS2_Ident) ||
                (rx[1] != PCIe_Types::TS1_Ident && rx[1] != PCIe_Types::TS2_Ident) 
            ){
                is_TS1 = (rx[1] == PCIe_Types::TS1_Ident);
                state = 0;
                is = (rx[1] == PCIe_Types::TS1_Ident) || (rx[1] == PCIe_Types::TS2_Ident);
                is_cons = is;
            }
            byteCounter = byteCounter + 2;
            break;
    }
}

void EIOS_Detector::Detect(uint16_t rxdata, uint8_t charisk, bool valid){
    uint8_t *rx = (uint8_t*)&rxdata;
    bool isk[2];
    uint8_t shift = 1;
    for (uint8_t i = 0; i<2; i++){
        isk[i] = (charisk&shift) > 0;
        shift = shift << 1; 
    }
    switch(state){
        case 0:
            is = 0;
            if ( (PCIe_Types::COM == rx[0]) && (isk[0]) &&
                 (PCIe_Types::IDL == rx[1]) && (isk[1]) && valid 
            )
            {
                state = state + 1;
            }else{
                is_cons = false;
            }
            break;
        case 1: // Check if TS1 or TS2

            is = (PCIe_Types::IDL == rx[0]) && (isk[0]) && (PCIe_Types::IDL == rx[1]) && (isk[1]);
            is_cons = is;
            state = 0;
            break;
    }
}

void EIEOS_Detector::Detect(uint16_t rxdata, uint8_t charisk, bool valid){
    uint8_t *rx = (uint8_t*)&rxdata;
    bool isk[2];
    uint8_t shift = 1;
    for (uint8_t i = 0; i<2; i++){
        isk[i] = (charisk&shift) > 0;
        shift = shift << 1; 
    }
    switch(state){
        case 0:
            is = 0;
            if ( (PCIe_Types::COM == rx[0]) && (isk[0]) &&
                 (PCIe_Types::EIE == rx[1]) && (isk[1]) && valid 
            )
            {
                ByteCounter = 0;
                state = state + 1;
            }else{
                is_cons = false;
            }
            break;
        case 1: // Check if TS1 or TS2
            if (ByteCounter <= 5 &&
                !((PCIe_Types::EIE == rx[0]) && (isk[0])) && 
                !((PCIe_Types::EIE == rx[1]) && (isk[1]))
            ){
                state = 0;
            }else if(ByteCounter >= 6){
                is = (PCIe_Types::EIE == rx[0]) && (isk[0]) && (PCIe_Types::TS1_Ident == rx[1]) && (~isk[1]);
                is_cons = is;
                state = 0;
            }else{
                state = 0;
            }
            
            break;
    }
}
