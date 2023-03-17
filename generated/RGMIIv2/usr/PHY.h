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
                virtual bool get_tx_data(uint8_t* data, bool* error, bool* nibbleend){return false;}; // return 1 when data is available
                virtual bool send_rx_data(uint8_t* data, bool* error, bool* col, uint16_t len){return false;};    // return is busy state
                virtual void reset_rx_idx(){};

        private:

    };
#endif
