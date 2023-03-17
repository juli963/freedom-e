
#include "Console_Defines.h"
#include "ETHClock.h"
/*
    Interfaces:
    MII #0
        10M  -> 2.5MHz @ 4Bits SDR
        100M -> 25MHz @ 4Bits SDR
    RMII #1
        10M  -> 50MHz @ 2Bits SDR
        100M -> 50MHz @ 2Bits SDR
    RGMII #2
        10M  -> 2.5MHz @ 4bits SDR
        100M -> 25MHz @ 4bits SDR
        1G   -> 125MHz @ 4bits DDR
    GMII #3
        10M  -> 2.5MHz @ 4bits SDR  // MII Fallback
        100M -> 25MHz @ 4bits SDR   // MII Fallback
        1G   -> 125MHz @ 8bits SDR
*/
bool uint_to_bool(uint8_t bin);

int main(int argc, char **argv) {
        uint16_t clock = CLK;
        uint8_t interface = Interface;
        bool error_n = true;

        ETHClock_TB *tb = new ETHClock_TB();

        #if TRACE == 1
                tb->opentrace("trace.vcd");
        #endif
        tb->m_core->io_PhyStat_Speed = 0;
        tb->init();

        ETHClock_TB::ETH_Speeds speed;
        bool out_ff;
        uint16_t counter = 0;
        
        float freq_should = 0;
        speed = ETHClock_TB::E_10MB;
        switch(speed){
                case ETHClock_TB::E_10MB:
                        if(interface == 1){     // RMII
                                freq_should = 50;
                        }else if(interface == 2){       // RGMII
                                freq_should = 1.25;
                        }else{
                                freq_should = 2.5;
                        }

                        tb->m_core->io_PhyStat_Speed = 0;
                        
                        tb->changed = 0;
                        while(tb->changed == 0){
                                tb->tick();
                        }
                        tb->changed = 0;
                        counter = 0;
                        while(tb->changed == 0){
                                tb->tick();
                                counter++;
                        }
                        if(tb->changed != 2){
                                tb->changed = 0;
                                while(tb->changed == 0){
                                        tb->tick();
                                        counter++;
                                }
                        }else{
                                counter = 1;
                        }
                        //printf("%i %i\n", counter, tb->changed );
                        tb->changed = 0;

                        if( ((float)clock/(float)counter) != freq_should ){
                                CRED
                                printf("Error Frequency is not correct. Clock = %i Interface = %i Speed = %i \n", clock, interface, speed);
                                CDEFAULT
                                error_n = false;
                        }

                        tb->init();
                        speed = ETHClock_TB::E_100MB;
                        
                case ETHClock_TB::E_100MB:
                        
                        if(interface == 1){     // RMII
                                freq_should = 50;
                        }else if(interface == 2){       // RGMII
                                freq_should = 12.5;
                        }else{
                                freq_should = 25;
                        }

                        tb->m_core->io_PhyStat_Speed = 1;
                        
                        tb->changed = 0;
                        while(tb->changed == 0){
                                tb->tick();
                        }
                        tb->changed = 0;
                        counter = 0;
                        while(tb->changed == 0){
                                tb->tick();
                                counter++;
                        }
                        if(tb->changed != 2){
                                tb->changed = 0;
                                while(tb->changed == 0){
                                        tb->tick();
                                        counter++;
                                }
                        }else{
                                counter = 1;
                        }
                        //printf("%i %i\n", counter, tb->changed );
                        tb->changed = 0;

                        if( ((float)clock/(float)counter) != freq_should ){
                                CRED
                                printf("Error Frequency is not correct. Clock = %i Interface = %i Speed = %i \n", clock, interface, speed);
                                CDEFAULT
                                error_n = false;
                        }

                        tb->init();
                        speed = ETHClock_TB::E_1G;
                case ETHClock_TB::E_1G: // Only (R)GMII 
                        if(interface >= 2){

                                freq_should = 125;

                                tb->m_core->io_PhyStat_Speed = 2;

                                tb->changed = 0;
                                while(tb->changed == 0){
                                        tb->tick();
                                }
                                tb->changed = 0;
                                counter = 0;
                                while(tb->changed == 0){
                                        tb->tick();
                                        counter++;
                                }
                                if(tb->changed != 2){
                                        tb->changed = 0;
                                        while(tb->changed == 0){
                                                tb->tick();
                                                counter++;
                                        }
                                }else{
                                        counter = 1;
                                }
                                tb->changed = 0;

                                //printf("%i %i\n", counter, tb->changed );

                                if( ((float)clock/(float)counter) != freq_should ){
                                        CRED
                                        printf("Error Frequency is not correct. Clock = %i Interface = %i Speed = %i \n", clock, interface, speed);
                                        CDEFAULT
                                        error_n = false;
                                }

                                tb->init();

                        };
                        break;
        }


        return 1; // Test passed
}

bool uint_to_bool(uint8_t bin){
        if(bin == 0){
                return false;
        }else{
                return true;
        }
}