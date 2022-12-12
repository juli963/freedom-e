
#include "Console_Defines.h"
#include "Litedram_test_model.h"

int main(int argc, char **argv) {

        Litedram_test_model_TB *tb = new Litedram_test_model_TB();

        tb->opentrace("trace.vcd");
        tb->init();

        for(uint32_t i = 0; i<100; i++){
                tb->tick();
        }
        tb->m_core->io_do_enq = 1;
        tb->m_core->io_do_deq = 1;

     
        for(uint32_t i = 0; i<1000; i++){
                tb->tick();
        }

        for(uint32_t i = 0; i<1000; i++){
                tb->tick();
        }

        if(tb->m_core->io_error == 0){
                return 1;
        }else{
                return 0;
        }
        
}
