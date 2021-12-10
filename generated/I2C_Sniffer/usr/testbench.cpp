
#include "Console_Defines.h"
#include "I2C_Slave.h"
#include "I2C_Host.h"
#include "I2C_Sniffer.h"


int main(int argc, char **argv) {

        I2C_Sniffer_TB *tb = new I2C_Sniffer_TB();
        I2C_Host host(tb);
        I2C_Slave slave(tb);

        tb->m_core->io_slave_scl_in = 1;
        tb->m_core->io_master_scl_in = 1;
        tb->m_core->io_slave_sda_in = 0;

        tb->opentrace("trace.vcd");
        tb->init();

        host.init();
        slave.init();

        I2C_Host::I2C_Header sHeader;
        sHeader.slave_address = 0b1000001;
        sHeader.rw = 1;
        sHeader.length = 2;
        host.fifo_header.push(sHeader);
        host.fifo_data.push(0x07);
        host.fifo_data.push(0x08);



        for(uint16_t i=0; i<10000; i++){
                host.tick();
                tb->realI2C();
                tb->tick();
        }
}
