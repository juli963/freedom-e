#include "Console_Defines.h"
#include "TLP_Module.h"

uint32_t CalculateTlpCRC(uint8_t* data, uint8_t len);

int main(int argc, char **argv) {

        TLP_Module_TB *tb = new TLP_Module_TB(0);
        TLP_Module_TB *tb2 = new TLP_Module_TB(1);

        tb2->opentrace("trace.vcd");
        tb->init();
        tb2->init();

	const char *state_names[11] = { "Detect", "Polling", 
                             "Configuration", "Recovery", "L0", "L0s", "L1", "L2", "Disabled", "Loopback", "Hot Reset" };

	enum e_Function{f_device=0, f_root=1};

	printf("Start as Root Port\n");

	tb2->m_core->io_pRootPort = f_device;
	tb->m_core->io_pRootPort = f_root;

        tb->m_core->io_sim_state = 0; 
        tb->m_core->io_scramble_enable = 0x0;
	tb2->m_core->io_scramble_enable = 0x0;

        for(uint32_t i = 0; i<20; i++){
                tb->m_core->io_GTP_data_rx_data  = 0;
                tb->m_core->io_GTP_data_rx_charisk  = 0;
                tb->tick();
	}

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

        for(uint32_t i=0; i<100;i++){
                tb->queue(tb2);
                tb2->queue(tb);  

                tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
        }
                tb2->m_core->io_data_intf_data_intf_rx_ready = 1;
                tb->m_core->io_data_intf_data_intf_rx_ready = 1;
        /*uint8_t tlp1_data_wire[] = { 0x00, 0x01, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21 };
        printf("Checksum= 0x%X\n", CalculateTlpCRC(&tlp1_data_wire[0], sizeof(tlp1_data_wire)));*/
        uint16_t tlp1_data[] = { 0x1211,0x1413,0x1615,0x1817,0x2019,0x2221 };
        tb->m_core->io_sim_state = 1; 
        tb2->m_core->io_sim_state = 1; 
       /* for(uint32_t p = 0; p<72;p++){
                for(uint32_t i=0; i< ((sizeof(tlp1_data)/2));){
                        tb->queue(tb2);
                        tb2->queue(tb); 

                        tb2->m_core->io_data_intf_data_intf_rx_ready = 1;
                        tb->m_core->io_data_intf_data_intf_rx_ready = 1;

                        tb2->m_core->io_data_intf_data_intf_tx_valid = 1;
                        tb->m_core->io_data_intf_data_intf_tx_valid = 1;
                        tb2->m_core->io_data_intf_data_intf_tx_bits_error = 0;
                        tb->m_core->io_data_intf_data_intf_tx_bits_error = 0;

                        if(i == (sizeof(tlp1_data)/2)-1){
                                tb2->m_core->io_data_intf_data_intf_tx_bits_strb = 1;
                                tb->m_core->io_data_intf_data_intf_tx_bits_strb = 1;
                        }else{
                                tb2->m_core->io_data_intf_data_intf_tx_bits_strb = 3;
                                tb->m_core->io_data_intf_data_intf_tx_bits_strb = 3;
                        }
                        
                        tb2->m_core->io_data_intf_data_intf_tx_bits_data = tlp1_data[i];
                        tb->m_core->io_data_intf_data_intf_tx_bits_data = tlp1_data[i];

                        tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                        tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                        tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                        tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;

                        if(tb->m_core->io_data_intf_data_intf_tx_ready > 0){
                                i++;
                        }
                }

                tb->queue(tb2);
                tb2->queue(tb); 

                tb2->m_core->io_data_intf_data_intf_tx_valid = 0;
                tb->m_core->io_data_intf_data_intf_tx_valid = 0;
                tb2->m_core->io_data_intf_data_intf_tx_bits_strb = 0;
                tb->m_core->io_data_intf_data_intf_tx_bits_strb = 0;

                tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
                
                tb->queue(tb2);
                tb2->queue(tb); 
                tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;

                tb->queue(tb2);
                tb2->queue(tb); 
                tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
        }*/
        

        for(uint32_t i=0; i<100000;i++){
                
                tb->queue(tb2);
                tb2->queue(tb); 
                if(i==1000){
                        tb->m_core->io_sim_state = 2; 
                        tb2->m_core->io_sim_state = 2; 
                } 
                if(i>1000){
                        tb2->m_core->io_GTP_data_rx_charisk  = tb->m_core->io_GTP_data_tx_charisk;
                        tb2->m_core->io_GTP_data_rx_data  = tb->m_core->io_GTP_data_tx_data;
                        tb->m_core->io_GTP_data_rx_charisk  = tb2->m_core->io_GTP_data_tx_charisk;
                        tb->m_core->io_GTP_data_rx_data  = tb2->m_core->io_GTP_data_tx_data;
                }
                tb2->m_core->io_data_intf_data_intf_tx_valid = 0;
                tb->m_core->io_data_intf_data_intf_tx_valid = 0;
                tb2->m_core->io_data_intf_data_intf_tx_bits_strb = 0;
                tb->m_core->io_data_intf_data_intf_tx_bits_strb = 0;

                
        }
}

uint32_t CalculateTlpCRC(uint8_t* data, uint8_t len)
{
	uint32_t poly = 0xedb88320;

	uint32_t crc = 0xffffffff;
	for(size_t n=0; n<len; n++)
	{
		uint8_t d = data[n];
		for(int i=0; i<8; i++)
		{
			bool b = ( crc ^ (d >> i) ) & 1;
			crc >>= 1;
			if(b)
				crc ^= poly;
		}
	}

	return ~(	((crc & 0x000000ff) << 24) |
				((crc & 0x0000ff00) << 8) |
				((crc & 0x00ff0000) >> 8) |
				 (crc >> 24) );
}