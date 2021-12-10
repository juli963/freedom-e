#include "I2C_Slave.h"

void I2C_Slave::init(){
    tb->m_core->io_slave_scl_in = 1;
    tb->m_core->io_slave_sda_in = 1;
    printf("I2C Slave Module Loaded.\n");
}

void I2C_Slave::tick(){
    
}