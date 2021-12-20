
#include "Console_Defines.h"
#include "TLP_w_Configuration.h"

#define TrainSim 0

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
        tb->m_core->io_scramble_enable = 0x0;
	tb2->m_core->io_scramble_enable = 0x0;

        /*for(uint32_t i = 0; i<20; i++){
                tb->m_core->io_GTP_data_rx_data  = 0;
                tb->m_core->io_GTP_data_rx_charisk  = 0;
                tb->tick();
	}*/
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
	for(uint32_t i = 0; i<10000000; i++){
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

        //tb->Create_Configuration_TLP(true ,false, false, 0xFF00, 0x01, 0xFF10, 0, 0x0F,0);
        uint8_t arr1[] = {
                0x74, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x50, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        tb->Create_Custom_TLP(arr1, sizeof(arr1));

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

        //tb->Create_Configuration_TLP(false ,false, false, 0xFF00, 0x01, 0xFF10, 0, 0x03,0);

        uint8_t arr2[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x1F, 0x01, 0x0C, 0x00, 0x00
        };
        tb->Create_Custom_TLP(arr2, sizeof(arr2));

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

        //tb->Create_Configuration_TLP(true ,false, false, 0xFF00, 0x01, 0xFF10, 0, 0x0F,0);

        uint8_t arr3[] = {
                0x0A, 0x00, 0x00, 0x00, 0x00, 0x10, 0x20, 0x04, 0x00, 0x00, 0x00, 0x00
        };
        tb->Create_Custom_TLP(arr3, sizeof(arr3));

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
                tb->Create_Configuration_TLP(true ,false, false, 0xFF00, 0x01, 0xFF10, 24, 0x0F,0);
                //tb->Create_Configuration_TLP(true ,false, false, 0xFF00, 0x01, 0xFF10, 13, 0x0F,0);
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

        /* 
        uint8_t arr4[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x01, 0x0F, 0x01, 0x00, 0x00, 0x2C
        };
        uint8_t arr5[] = {
                0x4A, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x10, 0x01, 0x00, 0x86, 0x80, 0xCA, 0xAB
        };
        
        
        // Array
        uint8_t arr6[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x11, 0x0F, 0x01, 0x00, 0x00, 0x00
        };
        uint8_t arr7[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x12, 0x0F, 0x01, 0x00, 0x00, 0x04
        };
        uint8_t arr8[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x13, 0x0F, 0x01, 0x00, 0x00, 0x08
        };
        uint8_t arr9[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x14, 0x0F, 0x01, 0x00, 0x00, 0x0C
        };
        uint8_t arr10[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x15, 0x0F, 0x01, 0x00, 0x00, 0x10
        };
        uint8_t arr11[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x16, 0x0F, 0x01, 0x00, 0x00, 0x14
        };
        uint8_t arr12[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x17, 0x0F, 0x01, 0x00, 0x00, 0x18
        };
        uint8_t arr13[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x18, 0x0F, 0x01, 0x00, 0x00, 0x1C
        };
        uint8_t arr14[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x19, 0x0F, 0x01, 0x00, 0x00, 0x20
        };
        uint8_t arr15[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x1A, 0x0F, 0x01, 0x00, 0x00, 0x24
        };
        uint8_t arr16[] = {
                0x04, 0x00, 0x00, 0x01, 0x00, 0x10, 0x1B, 0x0F, 0x01, 0x00, 0x00, 0x28
        };
  

        printf("\n\n");
        tb->decode_TLP(arr1, sizeof(arr1));
        printf("\n\n");
        tb->decode_TLP(arr2, sizeof(arr2));
        printf("\n\n");
        tb->decode_TLP(arr3, sizeof(arr3));
        printf("\n\n");
        tb->decode_TLP(arr4, sizeof(arr4));
        printf("\n\n");
        tb->decode_TLP(arr5, sizeof(arr5));

        printf("\n--------- Start Linux TLP's --------------\n");
        tb->decode_TLP(arr6, sizeof(arr6));
        printf("\n\n");
        tb->decode_TLP(arr7, sizeof(arr7));
        printf("\n\n");
        tb->decode_TLP(arr8, sizeof(arr8));
        printf("\n\n");
        tb->decode_TLP(arr9, sizeof(arr9));
        printf("\n\n");
        tb->decode_TLP(arr10, sizeof(arr10));
        printf("\n\n");
        tb->decode_TLP(arr11, sizeof(arr11));
        printf("\n\n");
        tb->decode_TLP(arr12, sizeof(arr12));
        printf("\n\n");
        tb->decode_TLP(arr13, sizeof(arr13));
        printf("\n\n");
        tb->decode_TLP(arr14, sizeof(arr14));
        printf("\n\n");
        tb->decode_TLP(arr15, sizeof(arr15));
        printf("\n\n");
        tb->decode_TLP(arr16, sizeof(arr16));
        printf("\n\n");
        */
#endif
}

