#include "testbench.h"
#include "../obj_dir/VMDIO.h"
#include "stdint.h"

#if !defined(MDIO_H)
    #define MDIO_H 1
    class        MDIO_TB : public TESTBENCH<VMDIO> {
        public:
            void init();
            void write(uint8_t phy, uint8_t reg, uint16_t data);
            uint16_t read_data();    // Get Data Back
            void read(uint8_t phy, uint8_t reg);

            bool is_busy();
            void set_start();
            void reset_start();
        private:

    };
#endif
