
#include "Console_Defines.h"
#include "MDIO.h"
#include "MDIO_PHY_sim.h"

#include <ctime>

#include <cstdlib>

using namespace std;

uint32_t create_rand(uint32_t min, uint32_t max);

int main(int argc, char **argv) {

        srand(time(0));  // Initialize random number generator.  

        bool error = false;
        uint8_t phyad = 5;

        MDIO_TB *tb = new MDIO_TB();
        MDIO_PHY_sim *phy_sim = new MDIO_PHY_sim(&tb->m_core->io_intf_MDIO_tri, &tb->m_core->io_intf_MDIO_in, &tb->m_core->io_intf_MDC, phyad, &error);

        #if TRACE == 1
                tb->opentrace("trace.vcd");
        #endif
        phy_sim->init();
        tb->init();

        /* Initialize Test Registers */
        uint16_t registers[32] = {0};
        for(uint8_t i = 0; i<32; i++){
                registers[i] = create_rand(1, 65536);
                printf("Reg #%i: 0x%04X \n", i, registers[i]);
        }

        for(uint8_t i = 0; i<100; i++){
                asm("NOP");
                phy_sim->main();
                tb->tick();
        }

        /* Write Registers incremental */
        printf("Write Registers incremental. \n");
        for(uint8_t i = 0; i<32; i++){         
                tb->write( phyad, i, registers[i] );
                phy_sim->main();
                tb->tick();
                while(!tb->is_busy()){
                        phy_sim->main();
                        tb->tick(); 
                }
                tb->reset_start();
                while(tb->is_busy()){
                        phy_sim->main();
                        tb->tick();       
                }
                if(tb->m_core->io_signals_error != 0){
                        CRED
                        printf("Write incremental Error Flag set at i=%i \n", i);
                        CDEFAULT
                        error = true;
                }
        }

        /* Read Back Registers incremental and Check data */
        printf("Read Back Registers incremental. \n");
        for(uint8_t i = 0; i<32; i++){         
                tb->read( phyad, i );
                phy_sim->main();
                tb->tick();
                while(!tb->is_busy()){
                        phy_sim->main();
                        tb->tick(); 
                }
                tb->reset_start();
                while(tb->is_busy()){
                        phy_sim->main();
                        tb->tick();       
                }
                if(tb->read_data() != registers[i]){
                        CRED
                        printf("Read Back incremental failed at i=%i \n", i);
                        CDEFAULT
                        error = true;
                }
                if(tb->m_core->io_signals_error != 0){
                        CRED
                        printf("Read Back incremental Error Flag set at i=%i \n", i);
                        CDEFAULT
                        error = true;
                }
        }

        /* Write all Registers 0 */
        printf("Write Registers 0. \n");
        for(uint8_t i = 0; i<32; i++){         
                tb->write( phyad, i, 0 );
                phy_sim->main();
                tb->tick();
                while(!tb->is_busy()){
                        phy_sim->main();
                        tb->tick(); 
                }
                tb->reset_start();
                while(tb->is_busy()){
                        phy_sim->main();
                        tb->tick();       
                }
                if(tb->m_core->io_signals_error != 0){
                        CRED
                        printf("Write 0 Error Flag set at i=%i \n", i);
                        CDEFAULT
                        error = true;
                }
        }

        /* Check if Registers are all 0 */
        printf("Check if Registers are all 0. \n");
        for(uint8_t i = 0; i<32; i++){         
                tb->read( phyad, i );
                phy_sim->main();
                tb->tick();
                while(!tb->is_busy()){
                        phy_sim->main();
                        tb->tick(); 
                }
                tb->reset_start();
                while(tb->is_busy()){
                        phy_sim->main();
                        tb->tick();       
                }
                if(tb->read_data() != 0){
                        CRED
                        printf("Read Back 0 failed at i=%i \n", i);
                        CDEFAULT
                        error = true;
                }
                if(tb->m_core->io_signals_error != 0){
                        CRED
                        printf("Read Back 0 Error Flag set at i=%i \n", i);
                        CDEFAULT
                        error = true;
                }
        }

        /* Write 100 Random Registers and read it back */
        printf("Write 100 Random Registers. \n");
        for(uint8_t i = 0; i<100; i++){    
                uint16_t rand_reg = create_rand(0,32);

                tb->write( phyad, rand_reg, registers[rand_reg] );
                phy_sim->main();
                tb->tick();
                while(!tb->is_busy()){
                        phy_sim->main();
                        tb->tick(); 
                }
                tb->reset_start();
                while(tb->is_busy()){
                        phy_sim->main();
                        tb->tick();       
                }
                if(tb->m_core->io_signals_error != 0){
                        CRED
                        printf("Write Random Error Flag set at i=%i \n", i);
                        CDEFAULT
                        error = true;
                }

                //phy_sim->main();
                //tb->tick();    

                // Read Back
                tb->read( phyad, rand_reg );
                phy_sim->main();
                tb->tick();
                while(!tb->is_busy()){
                        phy_sim->main();
                        tb->tick(); 
                }
                tb->reset_start();
                while(tb->is_busy()){
                        phy_sim->main();
                        tb->tick();       
                }
                if(tb->read_data() != registers[rand_reg]){
                        CRED
                        printf("Read Back Random failed at i=%i \n", i);
                        CDEFAULT
                        error = true;
                }
                if(tb->m_core->io_signals_error != 0){
                        CRED
                        printf("Read Back Random Error Flag set at i=%i \n", i);
                        CDEFAULT
                        error = true;
                }
        }

        /* Write 0 to Reg*/
        printf("Write 0 to Random Reg. \n");
        uint16_t rand_reg = create_rand(0,32);
        tb->write( phyad, rand_reg, registers[rand_reg] );
        phy_sim->main();
        tb->tick();
        while(!tb->is_busy()){
                phy_sim->main();
                tb->tick(); 
        }
        tb->m_core->io_signals_data_wr = 0;
        tb->reset_start();
        while(tb->is_busy()){
                phy_sim->main();
                tb->tick();       
        }
        if(tb->m_core->io_signals_error != 0){
                CRED
                printf("Write 0 at Random Error Flag set \n");
                CDEFAULT
                error = true;
        }

        /* Write Random Register and Change Values while Busy */
        printf("Write Random Reg and Change Value and Start while Busy. \n");
        tb->write( phyad, rand_reg, registers[rand_reg] );
        phy_sim->main();
        tb->tick();
        while(!tb->is_busy()){
                phy_sim->main();
                tb->tick(); 
        }
        tb->write( phyad, rand_reg, 0 );  
        while(tb->is_busy()){
                phy_sim->main();
                tb->tick();       
        }
        tb->reset_start();
        if(tb->m_core->io_signals_error != 0){
                CRED
                printf("Write Random at Random Reg Error Flag set \n" );
                CDEFAULT
                error = true;
        }
        // 12 Ticks -> Reset Start
        for(uint8_t i = 0; i<12; i++){    
                phy_sim->main();
                tb->tick();
        }

        /* Read Back data must be the old Register */
        printf("Read Back Random Reg. \n");
        tb->read( phyad, rand_reg );
        phy_sim->main();
        tb->tick();
        while(!tb->is_busy()){
                phy_sim->main();
                tb->tick(); 
        }
        tb->reset_start();
        while(tb->is_busy()){
                phy_sim->main();
                tb->tick();       
        }
        if(tb->read_data() != registers[rand_reg]){
                CRED
                printf("Read Back Random Data from Random Reg failed \n");
                CDEFAULT
                error = true;
        }
        if(tb->m_core->io_signals_error != 0){
                CRED
                printf("Read Back Random Data form Random Reg Error Flag set \n");
                CDEFAULT
                error = true;
        }

        /* Write to wrong Phyadress */
        printf("Write wrong Phy Random Reg. \n");     
        tb->write( phyad+2, rand_reg, 0 );
        phy_sim->main();
        tb->tick();
        while(!tb->is_busy()){
                phy_sim->main();
                tb->tick(); 
        }
        tb->reset_start();
        while(tb->is_busy()){
                phy_sim->main();
                tb->tick();       
        }
        if(tb->m_core->io_signals_error != 0){
                CRED
                printf("Write wrong Phyaddress Error Flag set \n");
                CDEFAULT
                error = true;
        }

        /* Check Back, data must be original */
        printf("Check if Register is not changed. \n");
        tb->read( phyad, rand_reg );
        phy_sim->main();
        tb->tick();
        while(!tb->is_busy()){
                phy_sim->main();
                tb->tick(); 
        }
        tb->reset_start();
        while(tb->is_busy()){
                phy_sim->main();
                tb->tick();       
        }
        if(tb->read_data() != registers[rand_reg]){
                CRED
                printf("Read Back wrong Phyaddress failed \n");
                CDEFAULT
                error = true;
        }
        if(tb->m_core->io_signals_error != 0){
                CRED
                printf("Read Back wrong Phyaddress Error Flag set \n");
                CDEFAULT
                error = true;
        }


        phy_sim->print_registers();

        if (error){
                CRED
                printf("Error on PHY Level!\n");
                CDEFAULT
                return false;
        }else{
                if(tb->m_core->io_signals_error == 1){
                        CRED
                        printf("Error Signal of MDIO Interface set!\n");
                        CDEFAULT
                        return false;
                }else{
                        return true;    // Everything OK
                }
        }
        
}

uint32_t create_rand(uint32_t min, uint32_t max){
        return min+ (rand() % max);
}