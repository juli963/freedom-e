#include "I2C_Simulator.h"

void I2C_Simulator::init(){
    tb->m_core->io_i2c_scl = 1;
    tb->m_core->io_i2c_sda = 1;

    printf("I2C Simulator Module Loaded.\n");
}

void I2C_Simulator::tick(){
    if(!fifo_header.empty()){
        if(temp >= period/2 || (temp >= period/4 && state == stop)){
            switch(state){
                case start:
                    data_count = 0;
                    if(!tb->m_core->io_i2c_sda){
                        tb->m_core->io_i2c_scl = 0;
                        state = slave_address;

                        printf("\nI2C Package to Send\n");
                        printf("\tTo Slave Address: 0x%02X\n", fifo_header.front().slave_address);
                        printf("\tRead: 0x%01X, Data Length: 0x%04X\n", fifo_header.front().rw, fifo_header.front().length);
                    }
                    
                    stop_done = false;
                    tb->m_core->io_i2c_sda = 0;
                    break;
                case slave_address:
                    tb->m_core->io_i2c_scl = ~tb->m_core->io_i2c_scl;
                    if(tb->m_core->io_i2c_scl){
                        uint8_t temp_address = reverse(fifo_header.front().slave_address)>>1;
                        tb->m_core->io_i2c_sda = (temp_address&(1<<counter))>>counter;
                        counter ++;
                    }
                    if(counter >= 7 && !tb->m_core->io_i2c_scl){
                        state = rw;
                        counter = 0;
                    }
                    break;
                case rw:
                    tb->m_core->io_i2c_scl = ~tb->m_core->io_i2c_scl;
                    if(tb->m_core->io_i2c_scl){
                        tb->m_core->io_i2c_sda = fifo_header.front().rw;
                        counter++;
                    }
                    if(!tb->m_core->io_i2c_scl && counter > 0){
                        state = address_ack;
                        counter = 0;
                    }
                    break;
                case address_ack:
                    tb->m_core->io_i2c_scl = ~tb->m_core->io_i2c_scl;
                    if(tb->m_core->io_i2c_scl){   // check if ACK = 0 -> Valid Address and data
                        tb->m_core->io_i2c_sda = 0;
                        counter++;
                    }

                    if(!tb->m_core->io_i2c_scl && counter > 0){
                        if(data_count >= fifo_header.front().length){
                            state = stop;
                        }else{
                            state = data;
                        }
                        counter = 0;
                    }
                    break;
                case ack:
                    tb->m_core->io_i2c_scl = ~tb->m_core->io_i2c_scl;
                    if(tb->m_core->io_i2c_scl){   // check if ACK = 0 -> Valid Address and data
                        tb->m_core->io_i2c_sda = 0;
                        counter++;
                    }

                    if(!tb->m_core->io_i2c_scl && counter > 0){
                        if(data_count >= fifo_header.front().length){
                            state = stop;
                        }else{
                            state = data;
                        }
                        counter = 0;
                    }
                    break;
                case data:
                    tb->m_core->io_i2c_scl = ~tb->m_core->io_i2c_scl;
                    if(tb->m_core->io_i2c_scl){
                       // if(fifo_header.front().rw == 0){
                            uint8_t temp_data = reverse(fifo_data.front());
                            tb->m_core->io_i2c_sda = (temp_data&(1<<counter))>>counter;
                            
                      /*   }else{
                            tb->m_core->io_i2c_sda = 1;
                        }*/
                        
                        counter ++;
                        
                    }
                    if(counter >= 8 && !tb->m_core->io_i2c_scl){
                        if(fifo_header.front().rw != 0){ // Read
                           tb->m_core->io_i2c_sda = 0;
                        }
                        state = ack;
                        data_count++;
                        counter = 0;
                        fifo_data.pop();
                    }
                    break;
                case stop:
                    
                    if(stop_done){
                        fifo_header.pop();
                        state = start;
                    }
                    if(tb->m_core->io_i2c_sda && !stop_done && tb->m_core->io_i2c_scl){
                        tb->m_core->io_i2c_sda = 0;
                    }else if(tb->m_core->io_i2c_scl == 1){
                        tb->m_core->io_i2c_sda = 1;
                        
                        stop_done = true;
                    }    
                    tb->m_core->io_i2c_scl = 1;                
                    break;
            } 
            temp = 0;
        }else{
            temp++;
        }
    }
}

uint8_t I2C_Simulator::reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}