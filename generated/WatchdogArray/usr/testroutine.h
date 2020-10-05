#include <stdint.h>
#include "watchdogreg.h"
#include "config.h"

class TestRoutines{

    public:
        bool glerror = false;
        TestRoutines(void){
            for(uint8_t i = 0; i<WD_cmpWidth; i++){
                max_cmp |= ((uint64_t)1<<i);
            }
        }
        void TestSingleWD(Watchdog_TB* tb,uint8_t Dog,uint8_t Output);
        void TestInversion(Watchdog_TB* tb);
        void checkEndtest(Watchdog_TB* tb, uint8_t Dog, uint8_t Output);

    private:
        uint8_t testnumber = 0;
        uint64_t max_cmp = 0;

};