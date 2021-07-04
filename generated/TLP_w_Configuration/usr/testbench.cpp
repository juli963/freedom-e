
#include "Console_Defines.h"
#include "TLP_w_Configuration.h"

#define TrainSim 1

int main(int argc, char **argv) {

        TLP_w_Configuration_TB *tb = new TLP_w_Configuration_TB(0);
        TLP_w_Configuration_TB *tb2 = new TLP_w_Configuration_TB(1);

        tb->opentrace("trace.vcd");
        tb->init();
        tb2->init();

        tb->m_core->io_sim_state = 0; 
        tb2->m_core->io_sim_state = 0; 

        const char *state_names[11] = { "Detect", "Polling", 
                             "Configuration", "Recovery", "L0", "L0s", "L1", "L2", "Disabled", "Loopback", "Hot Reset" };

	enum e_Function{f_device=0, f_root=1};

	printf("Start as Root Port\n");

	tb2->m_core->io_pRootPort = f_device;
	tb->m_core->io_pRootPort = f_root;

        tb->m_core->io_sim_state = 0; 
        tb->m_core->io_scramble_enable = 0x1;
	tb2->m_core->io_scramble_enable = 0x0;

        for(uint32_t i = 0; i<20; i++){
                tb->m_core->io_GTP_data_rx_data  = 0;
                tb->m_core->io_GTP_data_rx_charisk  = 0;
                tb->tick();
	}
#if TrainSim == 1
        for(uint8_t i = 0; i< 255; i++){
                tb->Create_Train_Set();
        }
        tb->Shift_FIFO();
        for(uint8_t i = 0; i< 255; i++){
                tb->Create_Train_Set();
        }
        for(uint32_t i=0; i<10000;i++){
                tb->queue(tb2);
                tb2->queue(tb);  
        }
#endif
#if TrainSim == 0
        uint8_t x = 0;
        uint32_t state_ff = 0;
	uint32_t substate_ff = 0;
	for(uint32_t i = 0; i<100000; i++){
                tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                if(tb->m_core->io_up > 0){
                        CGREEN
                        printf("Data Link is Up \n");
                        CDEFAULT
                        break;
                }else{
                        x = 0;
                        tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                        tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
                }
                

                state_ff = tb->m_core->io_ltssm_state;
                substate_ff = tb->m_core->io_ltssm_substate;
                
                tb->tick();
                tb2->tick();
                if(tb->m_core->io_ltssm_state != state_ff){
                        CGREEN
                        printf("State has Changed, now: %s \n", state_names[tb->m_core->io_ltssm_state]);
                        CDEFAULT
                }
                if(tb->m_core->io_ltssm_substate != substate_ff){
                        CGREEN
                        printf("Substate has Changed, now: %i \n", tb->m_core->io_ltssm_substate);
                        CDEFAULT
                }
	}

        /* 
                Data Link is Up !! 
                Start User Code here
        
         */
        tb->m_core->io_sim_state = 1; 
        tb2->m_core->io_sim_state = 1; 

        for(uint32_t i=0; i<100;i++){
                //tb->queue(tb2);
                //tb2->queue(tb);  
                tb->tick();
                tb2->tick();

                tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
        }

        tb->m_core->io_sim_state = 2; 
        tb2->m_core->io_sim_state = 2; 
        tb->ack = true; 

        tb->Create_Configuration_TLP(true ,false, false, 0xFF00, 0x01, 0xFF10, 0, 0x0F,0);

        tb2->m_core->io_data_intf_data_intf_rx_ready = 1;
        tb->m_core->io_data_intf_data_intf_rx_ready = 1;
        for(uint32_t i=0; i<200;i++){
                tb->queue(tb2);
                tb2->queue(tb);  

                /*tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;*/
        }

        tb->m_core->io_sim_state = 3; 
        tb2->m_core->io_sim_state = 3; 

        tb->Create_Configuration_TLP(false ,false, false, 0xFF00, 0x01, 0xFF10, 0, 0x03,0);

        for(uint32_t i=0; i<200;i++){
                tb->queue(tb2);
                tb2->queue(tb);  

                /*tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;*/
        }

        tb->m_core->io_sim_state = 4; 
        tb2->m_core->io_sim_state = 4; 

        tb->Create_Configuration_TLP(true ,false, false, 0xFF00, 0x01, 0xFF10, 0, 0x0F,0);

        for(uint32_t i=0; i<200;i++){
                tb->queue(tb2);
                tb2->queue(tb);  

                /*tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;*/
        }


        tb->m_core->io_sim_state = 5; 
        tb2->m_core->io_sim_state = 5; 

        tb->Create_Configuration_TLP(false ,false, false, 0xFF00, 0x01, 0xFF10, 0x04, 0x0F,0xFFFFFFFF);

        for(uint32_t i=0; i<200;i++){
                tb->queue(tb2);
                tb2->queue(tb);  

                /*tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;*/
        }
        
        tb->m_core->io_sim_state = 6; 
        tb2->m_core->io_sim_state = 6; 

        
        tb->Create_Configuration_TLP(true ,false, false, 0xFF00, 0x01, 0xFF10, 0x04, 0x0F,0);
        for(uint32_t i=0; i<200;i++){
                
                tb->queue(tb2);
                tb2->queue(tb);  

                /*tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;*/
        }

        tb->m_core->io_sim_state = 7; 
        tb2->m_core->io_sim_state = 7; 

        tb->ack = false; 
        tb->tick();
        for(uint32_t i=0; i<200;i++){
                tb->Create_Configuration_TLP(true ,false, false, 0xFF00, 0x01, 0xFF10, 0x04, 0x0F,0);
                for(uint32_t x = 0; x<10; x++){
                        tb->fill_rx(0,0);
                }
                tb->queue(tb2);
                tb2->queue(tb);  

                /*tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;*/
        }

        tb->m_core->io_sim_state = 8; 
        tb2->m_core->io_sim_state = 8; 
        tb->ack = true;
        for(uint32_t i=0; i<10000;i++){
                
                tb->queue(tb2);
                tb2->queue(tb);  

                /*tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;*/
        }
#endif
}

