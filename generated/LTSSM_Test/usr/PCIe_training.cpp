#include "PCIe_training.h"
#include "Console_Defines.h"
#include <cstdlib>

void PCIe_training::device_linkinit(){
    switch(storage->state){
        case PCIe_Types::Polling:
            cPolling->Work();
            break;
    }
    
}
