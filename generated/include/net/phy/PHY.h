#include <cstddef>
#include <stdint.h>

#if !defined(PHY_H)
        #define PHY_H 1

        class        PHY{
        public:
                PHY()
                { }
                virtual ~PHY()
                { }

                virtual void init(){};
                virtual uint16_t get_tx_data(uint8_t* data, bool* error, bool* nibbleend){return false;}; // return 1 when data is available
                virtual uint16_t send_rx_data(uint8_t* data, bool* error, bool* col, uint16_t len){return false;};    // return is busy state
                virtual void reset_rx_idx(){};

        private:

    };
#endif
