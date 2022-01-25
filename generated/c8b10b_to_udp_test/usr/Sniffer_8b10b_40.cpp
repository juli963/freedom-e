#include "Sniffer_8b10b_40.h"

void Sniffer_8b10b_40_TB::init(){
    printf("8b10b Module loaded.\n");
}

void Sniffer_8b10b_40_TB::enq_rx_fifo(uint8_t* data, uint8_t* isk, uint8_t length){
    //printf("Data to Fifo added. Length: %i \n", length);
    s8b10b v8b10b;
    for(uint8_t i = 0; i<length; i++){
        v8b10b.data = data[i];
        v8b10b.isk = isk[i];
        rx_fifo.push( v8b10b );
    }
}

void Sniffer_8b10b_40_TB::set_8b10b(s8b10b* p8b10b){
    uint64_t data = p8b10b[0].data | (p8b10b[1].data << 8) | (p8b10b[2].data << 16) | (p8b10b[3].data << 24);
    uint8_t isk = p8b10b[0].isk | (p8b10b[1].isk << 1) | (p8b10b[2].isk << 2) | (p8b10b[3].isk << 3);

    switch(m_core->io_regs_gtp_sel){
        case 0:
            m_core->io_data_in_0_bits_data = data;
            m_core->io_data_in_0_bits_charisk = isk;
            break;
        case 1:
            m_core->io_data_in_1_bits_data = data;
            m_core->io_data_in_1_bits_charisk = isk;
            break; 
        case 2:
            m_core->io_data_in_2_bits_data = data;
            m_core->io_data_in_2_bits_charisk = isk;
            break; 
        case 3:
            m_core->io_data_in_3_bits_data = data;
            m_core->io_data_in_3_bits_charisk = isk;
            break; 
    }
}

void Sniffer_8b10b_40_TB::deq_rx_fifo(){
    s8b10b data[4];
    for(uint8_t i=0; i<4; i++){
        if(!rx_fifo.empty()){
            data[i] = rx_fifo.front();
            //isk[i] = rx_fifo.front().isk;
            rx_fifo.pop();
            /*switch(i){
                case 0:
                    m_core->io_data_in_0_valid = 1;
                    break;
                case 1:
                    m_core->io_data_in_1_valid = 1;
                    break;
                case 2:
                    m_core->io_data_in_2_valid = 1;
                    break;
                case 3:
                    m_core->io_data_in_3_valid = 1;
                    break;
            }*/
            m_core->io_data_in_0_valid = 1;
            m_core->io_data_in_1_valid = 1;
            m_core->io_data_in_2_valid = 1;
            m_core->io_data_in_3_valid = 1;
       //}else{
        //    m_core->io_data_in_0_valid = 0;
        //    m_core->io_data_in_1_valid = 0;
        //    m_core->io_data_in_2_valid = 0;
        //    m_core->io_data_in_3_valid = 0;
            /*switch(i){
                case 0:
                    m_core->io_data_in_0_valid = 0;
                    break;
                case 1:
                    m_core->io_data_in_1_valid = 0;
                    break;
                case 2:
                    m_core->io_data_in_2_valid = 0;
                    break;
                case 3:
                    m_core->io_data_in_3_valid = 0;
                    break;
            }*/
        }
    }
    set_8b10b(data);
}

void Sniffer_8b10b_40_TB::enable_trigger_length(s8b10b start, uint16_t length, eTriggerMode mode, uint8_t num_trigger){
    switch(num_trigger){
        case 0:
            m_core->io_regs_trigger_mode_0 = mode;
            m_core->io_regs_trigger_start_0_data = start.data;
            m_core->io_regs_trigger_start_0_charisk = start.isk;
            m_core->io_regs_trigger_stop_0_data = length&0xFF;
            m_core->io_regs_trigger_stop_0_charisk = 0;
            break;
        case 1:
            m_core->io_regs_trigger_mode_1 = mode;
            m_core->io_regs_trigger_start_1_data = start.data;
            m_core->io_regs_trigger_start_1_data = start.isk;
            m_core->io_regs_trigger_stop_1_data = length&0xFF;
            m_core->io_regs_trigger_stop_1_charisk = 0;
            break; 
    }
}

void Sniffer_8b10b_40_TB::enable_trigger_code(s8b10b start, s8b10b stop, eTriggerMode mode, uint8_t num_trigger){
    switch(num_trigger){
        case 0:
            m_core->io_regs_trigger_mode_0 = mode;
            m_core->io_regs_trigger_start_0_data = start.data;
            m_core->io_regs_trigger_start_0_charisk = start.isk;
            m_core->io_regs_trigger_stop_0_data = stop.data;
            m_core->io_regs_trigger_stop_0_charisk = stop.isk;
            break;
        case 1:
            m_core->io_regs_trigger_mode_1 = mode;
            m_core->io_regs_trigger_start_1_data = start.data;
            m_core->io_regs_trigger_start_1_charisk = start.isk;
            m_core->io_regs_trigger_stop_1_data = stop.data;
            m_core->io_regs_trigger_stop_1_charisk = stop.isk;
            break; 
    }
}

void Sniffer_8b10b_40_TB::sel_gtp(uint8_t num){
    m_core->io_regs_gtp_sel = num;
}
