#include "Console_Defines.h"
#include "PRBS_Boolean_Switch.h"

int main(int argc, char **argv) {

        PRBS_Boolean_Switch_TB *tb = new PRBS_Boolean_Switch_TB();

        bool error = false;

        tb->opentrace("trace.vcd");
        tb->init();
        uint8_t init[] = {1,2,22,32};
        uint64_t seed = 0x55555555;
        tb->init_polynom(&init[0],4, 32);
        //printf("0x%lX \n", seed);
        
        //printf("0x%lX \n", seed);
        //tb->calculate_bool(seed);

        tb->reset();
        tb->tick();
        
        if(tb->m_core->io_out != 0){
                error = true;
                CRED;
                printf("Error, Output is 1 after Resetting.\n");
                CDEFAULT;
        }

        for(uint32_t i = 0; i<256; i++){
                tb->tick();
                if(tb->m_core->io_out != 0){
                        error = true;
                        CRED;
                        printf("Error, Output is 1 when no trigger is set.\n");
                        CDEFAULT;
                }
        }
        
        tb->m_core->io_trigger = 1;
        tb->tick();
        for(uint32_t i = 0; i<10000; i++){
                tb->tick();
                
                if(tb->calculate_bool(seed) != tb->m_core->io_out && !error){
                        error = true;
                        CRED;
                        printf("Error, Output is wrong at tick %li.\n", tb->m_tickcount);
                        CDEFAULT;
                }
                seed = tb->get_next_lfsr(seed);
        }

        if (error){
                return false;
        }else{
                return true;
        }
}
