#include "I2C_Host.h"

void I2C_Host::init(){
    tb->m_core->io_master_scl_in = 1;
    //output  io_master_scl_out,
    //output  io_master_scl_tri,
    tb->m_core->io_master_sda_in = 1;
    //output  io_master_sda_out,
    //output  io_master_sda_tri,

   /* input   io_slave_scl_in,
    output  io_slave_scl_out,
    output  io_slave_scl_tri,
    input   io_slave_sda_in,
    output  io_slave_sda_out,
    output  io_slave_sda_tri*/

    printf("I2C Host Module Loaded.\n");
}

void I2C_Host::tick(){
    if(!fifo_header.empty()){
        if(temp >= period/2 || (temp >= period/4 && state == stop)){
            switch(state){
                case start:
                    if(!tb->m_core->io_master_sda_in){
                        tb->m_core->io_master_scl_in = 0;
                        state = slave_address;
                    }
                    
                    stop_done = false;
                    tb->m_core->io_master_sda_in = 0;
                    break;
                case slave_address:
                    tb->m_core->io_master_scl_in = ~tb->m_core->io_master_scl_in;
                    if(tb->m_core->io_master_scl_in){
                        uint8_t temp_address = reverse(fifo_header.front().slave_address)>>1;
                        tb->m_core->io_master_sda_in = (temp_address&(1<<counter))>>counter;
                        counter ++;
                    }
                    if(counter >= 7 && !tb->m_core->io_master_scl_in){
                        state = rw;
                        counter = 0;
                    }
                    break;
                case rw:
                    tb->m_core->io_master_scl_in = ~tb->m_core->io_master_scl_in;
                    if(tb->m_core->io_master_scl_in){
                        tb->m_core->io_master_sda_in = fifo_header.front().rw;
                        counter++;
                    }
                    if(!tb->m_core->io_master_scl_in && counter > 0){
                        state = address_ack;
                        counter = 0;
                    }
                    break;
                case address_ack:
                    tb->m_core->io_master_scl_in = ~tb->m_core->io_master_scl_in;
                    if(tb->m_core->io_master_scl_in && !tb->m_core->io_master_sda_tri){   // check if ACK = 0 -> Valid Address and data
                        fail = false;
                    }else if(tb->m_core->io_master_scl_in && tb->m_core->io_master_sda_tri){ // Invalid Data or Address
                        fail = true;
                    }

                    if(!tb->m_core->io_master_scl_in){
                        
                        if(fail){
                            state = stop;
                        }else{
                            if(data_count >= fifo_header.front().length){
                                state = stop;
                            }else{
                                state = data;
                            }
                        }
                    }
                    break;
                case ack:
                    tb->m_core->io_master_scl_in = ~tb->m_core->io_master_scl_in;
                    if(fifo_header.front().rw == 0){ // Write
                        if(tb->m_core->io_master_scl_in && !tb->m_core->io_master_sda_out){   // check if ACK = 0 -> Valid Address and data
                            fail = false;
                        }else if(tb->m_core->io_master_scl_in && tb->m_core->io_master_sda_out){ // Invalid Data or Address
                            fail = true;
                        }else if(!tb->m_core->io_master_scl_in){
                            if(fail){
                                state = stop;
                            }else{
                                if(data_count >= fifo_header.front().length){
                                    state = stop;
                                }else{
                                    state = data;
                                }
                            }
                        }
                    }else{  // Read
                        tb->m_core->io_master_sda_in = 0;
                       /* if(tb->m_core->io_master_scl_in){
                            tb->m_core->io_master_sda_in = 0;
                        }*/
                        
                        if(!tb->m_core->io_master_scl_in){
                            if(data_count >= fifo_header.front().length){
                                state = stop;
                            }else{
                                state = data;
                            }
                        }
                    }
                    break;
                case data:
                    tb->m_core->io_master_scl_in = ~tb->m_core->io_master_scl_in;
                    if(tb->m_core->io_master_scl_in){
                        if(fifo_header.front().rw == 0){
                            uint8_t temp_data = reverse(fifo_data.front());
                            tb->m_core->io_master_sda_in = (temp_data&(1<<counter))>>counter;
                        }else{
                            tb->m_core->io_master_sda_in = 1;
                        }
                        
                        counter ++;
                        fifo_data.pop();
                    }
                    if(counter >= 8 && !tb->m_core->io_master_scl_in){
                        //tb->m_core->io_master_sda_in = 0;
                        if(fifo_header.front().rw != 0){ // Read
                           tb->m_core->io_master_sda_in = 0;
                        }
                        state = ack;
                        data_count++;
                        counter = 0;
                    }
                    break;
                case stop:
                    
                    if(stop_done){
                        fifo_header.pop();
                        state = start;
                    }
                    if(tb->m_core->io_master_sda_in && !stop_done && tb->m_core->io_master_scl_in){
                        tb->m_core->io_master_sda_in = 0;
                    }else if(tb->m_core->io_master_scl_in == 1){
                        tb->m_core->io_master_sda_in = 1;
                        
                        stop_done = true;
                    }    
                    tb->m_core->io_master_scl_in = 1;                
                    break;
            } 
            temp = 0;
        }else{
            temp++;
        }
    }
}

uint8_t I2C_Host::reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}