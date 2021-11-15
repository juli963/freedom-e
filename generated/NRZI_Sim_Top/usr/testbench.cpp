
#include "Console_Defines.h"
#include "NRZI_Sim_Top.h"

int main(int argc, char **argv) {

        NRZI_Sim_Top_TB *tb = new NRZI_Sim_Top_TB();

        tb->opentrace("trace.vcd");
        tb->init();
        
        uint8_t temp = 0;
        
        for(uint16_t i=0;i<1600;i++){
        	if(temp > 0 && tb->m_core->io_clock_sync== 0){
        		tb->m_core->io_data_in = rand() % 100;
        	}
        	temp = tb->m_core->io_clock_sync;
        	tb->tick();
        }
}
