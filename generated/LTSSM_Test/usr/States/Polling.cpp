#include "Polling.h"

void Polling::Work(){
    printf("%i \n", m_store->state);
    //m_store->state = PCIe_Types::Configuration;
}