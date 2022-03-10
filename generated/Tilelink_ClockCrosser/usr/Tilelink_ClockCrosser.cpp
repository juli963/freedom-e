
#include "Tilelink_ClockCrosser.h"

void Tilelink_ClockCrosser_TB::init(){
    reset();
        m_core->io_node_in_d_ready = 0;
        m_core->io_node_out_a_ready = 0;

        m_core->io_node_in_a_valid = 0;
        m_core->io_node_out_d_valid = 0;
        for (uint i = 0; i<32; i++){
                m_core->io_reset = 1;
                tick();
        }
        m_core->io_reset = 0;
    tick();
}

void Tilelink_ClockCrosser_TB::do_burst_write(){    // Only A with Data Telegrams -> Send first data then acknowledge

    tick();
    m_core->io_node_in_a_valid = 1;
    m_core->io_node_in_a_bits_address = 0xFF;
    m_core->io_node_in_a_bits_data = 0x01;
    tick();
    while( m_core->io_node_in_a_ready == 0 ){
        tick();
    }
    tick();
    m_core->io_node_in_a_bits_data += 0x01;
    m_core->io_node_out_a_ready = 1;
    tick();
    while( m_core->io_node_in_a_ready == 0  ){
        tick();
    }
    tick();
    m_core->io_node_in_a_bits_data += 0x01;
    tick();
    while( m_core->io_node_in_a_ready == 0  ){
        tick();
    }
    tick();
    m_core->io_node_in_a_bits_data += 0x01;
    m_core->io_node_in_a_valid = 0;
    tick();
    for(uint8_t i = 0; i<10; i++){
        tick();
    }
    tick();
    m_core->io_node_out_d_bits_data += 0x01;
    m_core->io_node_out_d_valid = 1;
    tick();
    for(uint8_t i = 0; i<10; i++){
        tick();
    }
    m_core->io_node_in_d_ready = 1;
    while( m_core->io_node_out_d_ready == 0  ){
        tick();
    }
    tick();
    m_core->io_node_out_d_bits_data += 0x01;
    m_core->io_node_out_d_valid = 1;
    tick();
    while( m_core->io_node_out_d_ready == 0  ){
        tick();
    }
    tick();
    m_core->io_node_out_d_bits_data += 0x01;
    m_core->io_node_out_d_valid = 0;
    tick();
    for(uint8_t i = 0; i<10; i++){
        tick();
    }
}

void Tilelink_ClockCrosser_TB::do_burst_read(){    // Only A with Data Telegrams -> Send first data then acknowledge

    tick();
    m_core->io_node_in_a_valid = 1;
    m_core->io_node_in_a_bits_address = 0xFF;
    m_core->io_node_in_a_bits_size = 0x01;
    m_core->io_node_in_a_bits_data = 0x01;
    tick();
    while( m_core->io_node_in_a_ready == 0 ){
        tick();
    }
    tick();
    m_core->io_node_in_a_valid = 0;
    m_core->io_node_out_a_ready = 1;
    tick();
    tick();
    tick();
    m_core->io_node_out_d_bits_data += 0x01;
    m_core->io_node_out_d_valid = 1;
    tick();
    for(uint8_t i = 0; i<10; i++){
        tick();
    }
    m_core->io_node_in_d_ready = 1;
    while( m_core->io_node_out_d_ready == 0  ){
        tick();
    }
    tick();
    m_core->io_node_out_d_bits_data += 0x01;
    m_core->io_node_out_d_valid = 1;
    tick();
    while( m_core->io_node_out_d_ready == 0  ){
        tick();
    }
    tick();
    m_core->io_node_out_d_bits_data += 0x01;
    m_core->io_node_out_d_valid = 0;
    tick();
    for(uint8_t i = 0; i<10; i++){
        tick();
    }
}
