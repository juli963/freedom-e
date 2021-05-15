#include "TLP_Module.h"
#include <queue>

void TLP_Module_TB::init(){
    reset();

    m_core->io_GTP_data_rx_disperr = 0;
    m_core->io_GTP_data_rx_notintable = 0;
    for (uint i = 0; i<32; i++){
            m_core->reset = 1;
            tick();
    }
    m_core->reset = 0;
    tick();
}

void TLP_Module_TB::queue(TLP_Module_TB* otb){
    /*if(otb->m_core->io_data_intf_data_intf_tx_valid && otb->m_core->io_data_intf_data_intf_tx_ready){
        qdata.push(otb->m_core->io_data_intf_data_intf_tx_bits_data);
        qstrb.push(otb->m_core->io_data_intf_data_intf_tx_bits_strb);
    }*/
    if(otb->m_core->io_send_valid && otb->m_core->io_send_ready){
        qdata.push(otb->m_core->io_send_bits);
        qstrb.push(otb->m_core->io_send_strb);
    }

    if(m_core->io_data_intf_data_intf_rx_valid && m_core->io_data_intf_data_intf_rx_ready){
        if(!qdata.empty()){
            printf("1");
            uint16_t data = qdata.front();
            uint16_t strb = qstrb.front();
            qdata.pop();
            qstrb.pop();
            uint16_t mask = 0xFFFF;
            if(strb == 1){
                mask = 0xFF;
            }
            if( (data&mask) != (m_core->io_data_intf_data_intf_rx_bits_data&mask) ){
                m_core->io_sim_state = m_core->io_sim_state + 1;
                printf("Data Error at SimState %i and TB Id = %i // Expected: 0x%X Received: 0x%X\n", m_core->io_sim_state, id, data, m_core->io_data_intf_data_intf_rx_bits_data);
            }
            if(strb != m_core->io_data_intf_data_intf_rx_bits_strb){
                m_core->io_sim_state = m_core->io_sim_state + 1;
                printf("Strb Error at SimState %i and TB Id = %i \n", m_core->io_sim_state, id);
            }
            //qdata.push(m_core->io_data_intf_data_intf_tx_bits_data);
            //qstrb.push(m_core->io_data_intf_data_intf_tx_bits_strb);
        }else{
            printf("FIFO Empty but receiving Data at TB Id = %i\n", id);
        }
            
    }

   tick();
}
