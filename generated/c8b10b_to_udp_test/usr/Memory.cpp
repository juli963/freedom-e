#include "Memory.h"

//#define DEBUG // Couple TX Back to RX


void Memory::init(){
    printf("Memory Module included\n");
    #if defined(Memory_Native)
        printf("\t --Native Interface included\n");
    #endif
    #if defined(Memory_FIFO)
        printf("\t --FIFO Interface included\n");
    #endif
    mem_data.resize(depth);
}

void Memory::tick(){  
    tick_native();
    tick_fifo();
}

void Memory::tick_native(){
    tb->m_core->io_native_cmd_ready = 1;
    if(tb->m_core->io_native_cmd_we > 0 && tb->m_core->io_native_cmd_valid > 0){
        idx = tb->m_core->io_native_cmd_addr;
        //printf("Address is Set to: 0x%X \n", idx);
    }

    if(tb->m_core->io_native_rdata_ready > 0){
        tb->m_core->io_native_rdata_valid = 1;
        if(idx < mem_data.size()){
            tb->m_core->io_native_rdata_data[0] = mem_data[idx] & 0x00000000FFFFFFFF;
            tb->m_core->io_native_rdata_data[1] = (mem_data[idx] & 0xFFFFFFFF00000000)>>32;
           // printf("Data at %i is read: 0x%X \n", idx, mem_data[idx]);
        }
    }else{
        tb->m_core->io_native_rdata_valid = 0;
    }

    if(tb->m_core->io_native_wdata_valid){
        tb->m_core->io_native_wdata_ready = 1;
        if(idx < mem_data.size()){
            mem_data[idx] = tb->m_core->io_native_wdata_data[0] | ((uint64_t)tb->m_core->io_native_wdata_data[1]<<32);
            //printf("Data at %i is written: 0x%lX \n", idx, mem_data[idx]);
        }
    }else{
        tb->m_core->io_native_wdata_ready = 0;
    }
   
/*
    io_native_cmd_valid
    io_native_cmd_ready
    io_native_cmd_we
    io_native_wdata_valid
    io_native_wdata_ready
    io_native_rdata_valid
    io_native_rdata_ready

    io_native_cmd_addr
    io_native_wdata_we
    io_native_wdata_data
    io_native_rdata_data*/


}

void Memory::tick_fifo(){
   /* io_fifo_ready
    io_fifo_valid
    io_fifo_bits*/
    if((uint16_t)tx_fifo_data.size() > (uint16_t)0xFFF){
        printf("Memory Problem, FIFO Size too much. \n");
    }else{
        tb->m_core->io_fifo_in_ready = 1;
        if (tb->m_core->io_fifo_in_valid > 0){
            tx_fifo_data.push(tb->m_core->io_fifo_in_bits[0] | ((uint64_t)tb->m_core->io_fifo_in_bits[1]<<32));  
            tx_fifo_data.push(tb->m_core->io_fifo_in_bits[2] | ((uint64_t)tb->m_core->io_fifo_in_bits[3]<<32));  
        }

        if( (fifo_ready_ff > 0 || triggered) && !tx_fifo_data.empty()){
            if(deq_en){
                tb->m_core->io_fifo_out_valid = 1;
            
                //printf("0x%016lX %i\n", tx_fifo_data.front(), tx_fifo_data.empty());
                for(uint8_t i = 0; i<2; i++){
                    if(!tx_fifo_data.empty()){
                        tb->m_core->io_fifo_out_bits[(i*2)+0] = tx_fifo_data.front()&0x00000000FFFFFFFF;
                        tb->m_core->io_fifo_out_bits[(i*2)+1] = (tx_fifo_data.front()&0xFFFFFFFF00000000)>>32;  
                        tx_fifo_data.pop();
                    }else{
                        tb->m_core->io_fifo_out_bits[(i*2)+0] = 0;
                        tb->m_core->io_fifo_out_bits[(i*2)+1] = 0;  
                    }
                }  
                triggered = false;
            }else{
                triggered = true;
            }
        }else{
            tb->m_core->io_fifo_out_valid = 0;
        }
        fifo_ready_ff = tb->m_core->io_fifo_out_ready;
    }

}

/*  Type(8, 0) Length(31, 8 ), Flags(63, 32), Checksum(79, 64), RSVD(127, 80) */
void Memory::add_data(uint8_t type, uint64_t flags, uint16_t length, uint8_t* data, uint64_t timestamp_start, uint64_t timestamp_end){
    uint64_t temp = type | ((uint64_t)length<<8) | ((uint64_t)(flags&0xFFFFFFFF)<<32);
    tx_fifo_data.push( temp );
    
    temp = ((uint64_t)(flags&0xFFFFFF00000000)>>32);
    tx_fifo_data.push( temp );  // We Ignore Checksum and Reserved 

    tx_fifo_data.push( timestamp_start );
    tx_fifo_data.push( timestamp_end );

    for(uint16_t i = 0; i<length; i+= 8){
        temp = 0;
        for(uint8_t x = 0; x<8; x++){
            if( (x+i) < (length)){
                temp |= ((uint64_t)data[i+x] << (x*8));
            }
        }
        //printf("0x%016lX \n", temp);
        tx_fifo_data.push( temp );
    }
    //printf("%li \n", tx_fifo_data.size());
    
    mem_data[0] += 1;
}