
#include "testbench.h"
#include "../obj_dir/VInbandDecoder.h"
#include "stdint.h"

#if !defined(InbandDecoder_H)
    #define InbandDecoder_H 1
    class        InbandDecoder_TB : public TESTBENCH<VInbandDecoder> {
        public:
            void init();
            void setStats(uint8_t speed, bool link, bool duplex);
            void setValid();
            void resetValid();

        private:

    };
#endif
