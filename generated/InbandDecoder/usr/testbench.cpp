
#include "Console_Defines.h"
#include "InbandDecoder.h"

#include <ctime>

#include <cstdlib>

using namespace std;

uint8_t create_rand(uint8_t min, uint8_t max);

int main(int argc, char **argv) {

        srand(time(0));  // Initialize random number generator.  

        InbandDecoder_TB *tb = new InbandDecoder_TB();

        tb->opentrace("trace.vcd");
        tb->init();

        uint8_t speed = 0;
        uint8_t link = 0;
        uint8_t duplex = 0;
        bool error_n = true;

        for(uint8_t i=0;i<create_rand(100,255);i++){
                speed = create_rand(0,4);
                link = create_rand(0,2);
                duplex = create_rand(0,2);
                tb->setStats(speed, (bool) link, (bool) duplex);
                tb->resetValid();
                for(uint8_t x = 0; x<create_rand(3,255);x++){
                        tb->tick();
                        tb->setValid();
                        tb->setStats(speed+1, (bool)(link+1), (bool)(duplex+1));
                }
                // Check if PHYStat has right data
                if(tb->m_core->io_PhyStat_Speed != speed){
                        CRED
                        printf("PhyStat is not equal selected Speed, Speed = 0x%X Link = 0x%X Duplex = 0x%X !\n", speed, link, duplex);
                        CDEFAULT
                        error_n = false;
                }
                if(tb->m_core->io_PhyStat_Link != link){
                        CRED
                        printf("PhyStat is not equal selected Speed, Speed = 0x%X Link = 0x%X Duplex = 0x%X !\n", speed, link, duplex);
                        CDEFAULT
                        error_n = false;
                }
                if(tb->m_core->io_PhyStat_Duplex != duplex){
                        CRED
                        printf("PhyStat is not equal selected Speed, Speed = 0x%X Link = 0x%X Duplex = 0x%X !\n", speed, link, duplex);
                        CDEFAULT
                        error_n = false;
                }

        }

        return 1; // Test passed
}

uint8_t create_rand(uint8_t min, uint8_t max){
        return min+(rand() % max);
}

/*  
val io = IO(new Bundle{
        val PhyStat = new PHYStat()
        val RXClock = Input(Clock())
        val RX_Data_Rising = Input(Vec(4,Bool()))
        val RX_CTL_Rising = Input(Bool())
})  
        val wLink = Wire(Bool())
        val wSpeed = Wire(UInt(4.W))
        val wDuplex = Wire(Bool())
*/