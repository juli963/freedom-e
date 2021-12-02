#if !defined(ESP_FIFO_Test_H)
    #include "testbench.h"
    #include "../obj_dir/VESP_FIFO_Test.h"
    #include "stdint.h"
    #include <queue>

    #define ESP_FIFO_Test_H 1
    class        ESP_FIFO_Test_TB : public TESTBENCH<VESP_FIFO_Test> {

        public:
            struct ESP_data {   
                uint8_t type;
                uint8_t ident;
                uint8_t cmd;
                uint8_t address;
                uint32_t data;
            };
            uint32_t rx_start_data = 0;
            std::queue <ESP_data> esp_rx_fifo;

            uint32_t tx_start_data = 0;
            std::queue <ESP_data> esp_tx_fifo;
            void init();
            void add_rx_fifo();
            void add_tx_fifo(uint8_t type, uint8_t ident, uint8_t cmd, uint8_t address, uint32_t data);
            void send_back();
            void check_rx_fifo_tick();
            void check_tx_fifo_tick();
        private:

    };
#endif
