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

void TLP_Module_TB::queue(){
    if(m_core->io_data_intf_data_intf_tx_valid && m_core->io_data_intf_data_intf_tx_ready){
        qdata.push(m_core->io_data_intf_data_intf_tx_bits_data);
        qstrb.push(m_core->io_data_intf_data_intf_tx_bits_strb);
    }

    if(m_core->io_data_intf_data_intf_rx_valid && m_core->io_data_intf_data_intf_rx_ready){
        uint16_t data = qdata.front();
        uint16_t strb = qstrb.front();
        qdata.pop();
        qstrb.pop();
        if(data != m_core->io_data_intf_data_intf_rx_bits_data){
            m_core->io_sim_state = m_core->io_sim_state + 1;
            printf("Data Error at SimState %i \n", m_core->io_sim_state);
        }
        if(strb != m_core->io_data_intf_data_intf_rx_bits_strb){
            m_core->io_sim_state = m_core->io_sim_state + 1;
            printf("Strb Error at SimState %i \n", m_core->io_sim_state);
        }
        //qdata.push(m_core->io_data_intf_data_intf_tx_bits_data);
        //qstrb.push(m_core->io_data_intf_data_intf_tx_bits_strb);
    }

   tick();
}
