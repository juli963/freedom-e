
#include "testbench.h"
#include "../obj_dir/VSGMII.h"
#include "stdint.h"

#if !defined(SGMII_H)
    #define SGMII_H 1
    class        SGMII_TB : public TESTBENCH<VSGMII> {
        public:
            void init();
            void set_speed(uint8_t speed);
            void set_link(uint8_t link);

            uint8_t speed = 0;
            uint8_t link = 0;
        private:

    };
#endif
