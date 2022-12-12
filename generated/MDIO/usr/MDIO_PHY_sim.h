#include <stdint.h>
#if !defined(MDIO_PHY_sim_H)
    #define MDIO_PHY_sim_H 1
    //#define MDIO_PHY_DEBUG 0
    class        MDIO_PHY_sim {
        public:
            MDIO_PHY_sim(uint8_t* mdio_tri_, uint8_t* mdio_in_, uint8_t* mclk_, uint8_t phyad, bool* error):
                mdio_tri(mdio_tri_),
                mdio_in(mdio_in_),
                mclk(mclk_),
                const_phyad (phyad),
                berror (error)
                {}
            void main();
            void init();
            void convert_mdio_tri();
            void print_registers();

        private:
            uint8_t* mdio_tri;
            uint8_t mdio_tri_con;
            uint8_t* mdio_in;
            uint8_t* mclk;
            uint8_t mclk_ff;
            uint32_t temp;

            uint8_t phyaddress; 
            uint8_t regaddress; 
            uint8_t opcode; 
            uint16_t data; 
            uint8_t bitcounter; 

            uint8_t const_phyad;
            bool* berror;

            uint16_t registers[32] = {0};

            enum MDIO_States
            {
                e_idle,
                e_start,
                e_opcode,
                e_phyadress,
                e_regadress,
                e_turnaround,
                e_data,
                e_end
            } state;

            bool clock_rising();
            bool clock_falling();
            bool is_write();
            bool is_phy();
            void set_error();
    };
#endif
