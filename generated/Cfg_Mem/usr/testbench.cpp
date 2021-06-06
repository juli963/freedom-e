
#include "Console_Defines.h"
#include "Cfg_Mem.h"

int main(int argc, char **argv) {

        Cfg_Mem_TB *tb = new Cfg_Mem_TB();

        tb->opentrace("trace.vcd");
        tb->init();
        
        tb->m_core->io_MemRX_Addr = 1;
        tb->tick();
        for(uint8_t i=0; i<16; i++){
		tb->m_core->io_MemRX_Addr = i;
		tb->tick();
        }
        
        for(uint8_t i=4; i<=9;i++){
		tb->m_core->io_MemRX_Addr = i;
		tb->m_core->io_MemRX_wrData = 0xADFF;
		tb->m_core->io_MemRX_wrStrb = 0x07;
		tb->tick();
		
		tb->m_core->io_MemRX_Addr = i;
		tb->m_core->io_MemRX_wrData = 0xFFFFFFFF;
		tb->m_core->io_MemRX_wrStrb = 0xF;
		tb->tick();
		
		tb->m_core->io_MemRX_Addr = i;
		tb->m_core->io_MemRX_wrData = 0xFFFFFFFF;
		tb->m_core->io_MemRX_wrStrb = 0x0;
		tb->tick();
	}
}
