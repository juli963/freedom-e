#include "Console_Defines.h"
#include "MII.h"
#include "MII_PHY.h"
#include "PHY_Bus2.h"

#include <ctime>

#include <cstdlib>

using namespace std;

uint32_t create_rand(uint32_t min, uint32_t max);

void bus_to_phy( MII_TB* tb, MII_PHY *phy, PHY_Bus2 *bus, bool* berror );
void phy_to_bus( MII_TB* tb, MII_PHY *phy, PHY_Bus2 *bus, bool* berror, bool with_Col );

int main(int argc, char **argv) {
        srand(time(0));  // Initialize random number generator.  

        bool error_n = true;


        MII_TB *tb = new MII_TB();

        MII_PHY::rx_signals phy_signals_rx = {
                .RXCLK = &tb->m_core->io_intf_rx_RXCLK,
                .RXDV = &tb->m_core->io_intf_rx_RXDV, 
                .RXER = &tb->m_core->io_intf_rx_RXER, 
                .RXD = &tb->m_core->io_intf_rx_RXD,
                .CRS = &tb->m_core->io_intf_rx_CRS,
                .COL = &tb->m_core->io_intf_rx_COL
        };

        MII_PHY::tx_signals phy_signals_tx = {
                .TXCLK = &tb->m_core->io_intf_tx_TXCLK,
                .TXEN = &tb->m_core->io_intf_tx_TXEN, 
                .TXER = &tb->m_core->io_intf_tx_TXER, 
                .TXD = &tb->m_core->io_intf_tx_TXD
        };

        PHY_Bus2::tx_signals bus_signals_tx = {
                .valid = &tb->m_core->io_Bus_tx_valid ,
                .strb = &tb->m_core->io_Bus_tx_bits_strb ,
                .bits = &tb->m_core->io_Bus_tx_bits_bits,
                .err = &tb->m_core->io_Bus_tx_bits_err
        };
        PHY_Bus2::rx_signals bus_signals_rx = {
                .valid = &tb->m_core->io_Bus_rx_valid ,
                .strb = &tb->m_core->io_Bus_rx_bits_strb ,
                .bits = &tb->m_core->io_Bus_rx_bits_bits,
                .err = &tb->m_core->io_Bus_rx_bits_err
        };

        MII_PHY *phy = new MII_PHY(phy_signals_tx, phy_signals_rx);
        PHY_Bus2 *bus = new PHY_Bus2(&tb->m_core->io_Bus_tx_clock, &tb->m_core->io_Bus_rx_clock, bus_signals_tx, bus_signals_rx, &tb->m_core->io_Bus_crs);

        tb->opentrace("trace.vcd");
        tb->init();
        phy->init();
        bus->init();
        tb->change_speed(0);

        bus->reset_tx_idx();
        phy->reset_rx_idx();

        //uint8_t tx_data[8] = {0xFF,0xFA,0xAA,0xAB,0x55,0x54,0x69,0x87};
        //bool tx_error[8] = {false,false,true,false,false,true,false,false};

        //uint8_t phy_tx_data = 0;
        //bool phy_tx_error = false;
        //bool phy_tx_nibbleend = false;
//bool MII_PHY::get_tx_data(uint8_t* data, bool* error, bool* nibbleend)

        /*while( bus->send_tx_data(tx_data,tx_error,8) ){
                tb->tick();
                if(phy->get_tx_data(&phy_tx_data, &phy_tx_error, &phy_tx_nibbleend)){
                        printf(" Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_tx_data, (uint8_t) phy_tx_error, (uint8_t) phy_tx_nibbleend);
                        phy_tx_error = false;
                }
        }



        for(uint16_t i = 0; i<1024; i++){
                tb->tick();
                if(phy->get_tx_data(&phy_tx_data, &phy_tx_error, &phy_tx_nibbleend)){
                        printf(" Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_tx_data, (uint8_t) phy_tx_error, (uint8_t) phy_tx_nibbleend);
                        phy_tx_error = false;
                }
        }*/

        /* Standard Transaction */
        bus_to_phy( tb, phy, bus, &error_n );
        phy_to_bus( tb, phy, bus, &error_n, true );
        

        /* Check CRS */
        tb->m_core->io_intf_rx_CRS = 1;
        bus_to_phy( tb, phy, bus, &error_n );
        phy_to_bus( tb, phy, bus, &error_n, true );
        if(tb->m_core->io_Bus_crs == 0){
                CRED
                printf("ERR_CRS CRS not going high. \n");
                CDEFAULT
                error_n = false;
        }

        /* Speed Change */
        tb->change_speed(1);
        bus_to_phy( tb, phy, bus, &error_n );
        phy_to_bus( tb, phy, bus, &error_n, true );
        
        /* Check CRS */
        tb->m_core->io_intf_rx_CRS = 0;
        bus_to_phy( tb, phy, bus, &error_n );
        phy_to_bus( tb, phy, bus, &error_n, true );
        if(tb->m_core->io_Bus_crs > 0){
                CRED
                printf("ERR_CRS CRS not going down. \n");
                CDEFAULT
                error_n = false;
        }
        /* Multi Transaction 100x?*/
        for(uint8_t i = 0; i<100; i++){
                phy_to_bus( tb, phy, bus, &error_n, true );
        }
        for(uint8_t i = 0; i<100; i++){
                bus_to_phy( tb, phy, bus, &error_n );
        }
        for(uint8_t i = 0; i<100; i++){
                bus_to_phy( tb, phy, bus, &error_n );
                phy_to_bus( tb, phy, bus, &error_n, true );
        }

        return error_n;
}

uint32_t create_rand(uint32_t min, uint32_t max){       // Max nur bis, nicht einschließlich
        return min+ (rand() % max);
}

void bus_to_phy( MII_TB* tb, MII_PHY* phy, PHY_Bus2* bus, bool* berror ){
        
        uint16_t length = create_rand(32, 512);

        uint8_t data[length] = {0};
        bool error[length] = {false};

        uint8_t phy_tx_data = 0;
        uint16_t phy_recv_idx=0;
        bool phy_tx_error = false;
        bool phy_tx_nibbleend;

        for(uint16_t i = 0; i<length;i++){
                data[i] = create_rand(0, 256);
                error[i] = create_rand(0, 2) > 0;
        }
        
        bus->send_tx_data(data,error,length);
        while( bus->send_tx_data(data,error,length) ){
                tb->tick();
                if(phy->get_tx_data(&phy_tx_data, &phy_tx_error, &phy_tx_nibbleend)){
                        //printf(" Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_tx_data, (uint8_t) phy_tx_error, (uint8_t) phy_tx_nibbleend);
                        if(phy_tx_error != error[phy_recv_idx]){
                                CRED
                                printf("ERROR Bit @idx %i Data send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data, phy_tx_error, phy_tx_nibbleend);
                                CDEFAULT
                                *berror = false;
                        }
                        if(phy_tx_data != data[phy_recv_idx]){
                                CRED
                                printf("ERROR Data Data @idx %i send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data, phy_tx_error, phy_tx_nibbleend);
                                CDEFAULT
                                *berror = false;
                        }
                        if(phy_tx_nibbleend){
                                CRED
                                printf("ERROR Nibble @idx %i Data send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data, phy_tx_error, phy_tx_nibbleend);
                                CDEFAULT
                                *berror = false;
                        }
                        phy_recv_idx++;
                        phy_tx_error = false;
                }
        }

        uint16_t speed_delay = 800;     // 800 Ticks at 10MB/s
        if(tb->speed == 1){
                speed_delay = 80;
        }

        for(uint16_t i = 0; i<speed_delay; i++){
                tb->tick();
                if(phy->get_tx_data(&phy_tx_data, &phy_tx_error, &phy_tx_nibbleend)){
                        //printf(" Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_tx_data, (uint8_t) phy_tx_error, (uint8_t) phy_tx_nibbleend);
                        if(phy_tx_error != error[phy_recv_idx]){
                                CRED
                                printf("ERROR Bit Data @idx %i send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data, phy_tx_error, phy_tx_nibbleend);
                                CDEFAULT
                                *berror = false;
                        }
                        if(phy_tx_data != data[phy_recv_idx]){
                                CRED
                                printf("ERROR Data Data @idx %i send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data, phy_tx_error, phy_tx_nibbleend);
                                CDEFAULT
                                *berror = false;
                        }
                        if(phy_tx_nibbleend){
                                CRED
                                printf("ERROR Nibble @idx %i Data send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data, phy_tx_error, phy_tx_nibbleend);
                                CDEFAULT
                                *berror = false;
                        }
                        phy_recv_idx++;
                        phy_tx_error = false;
                }
        }
        bus->reset_tx_idx();
}

void phy_to_bus( MII_TB* tb, MII_PHY* phy, PHY_Bus2* bus, bool* berror, bool with_Col ){
        
        uint16_t length = create_rand(32, 512);

        uint8_t data[length] = {0};
        bool error[length] = {false};
        bool col[length] = {false};

        uint8_t bus_rx_data;
        uint16_t bus_recv_idx=0;
        bool bus_rx_error;

        for(uint16_t i = 0; i<length;i++){
                data[i] = create_rand(0, 256);
                error[i] = create_rand(0, 2) > 0;
                col[i] = create_rand(0, 2) > 0;
        }
        
        phy->send_rx_data(data,error, col,length);
        while( phy->send_rx_data(data,error, col,length) ){
                tb->tick();
                if(bus->get_rx_data(&bus_rx_data, &bus_rx_error)){
                        //printf(" Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_tx_data, (uint8_t) phy_tx_error, (uint8_t) phy_tx_nibbleend);
                        if(bus_rx_error != error[bus_recv_idx] && bus_rx_error != col[bus_recv_idx]){
                                CRED
                                printf("ERROR Bit Data @idx %i send: 0x%02X Error %i Col %i Data Received: 0x%02X Error: %i \n", bus_recv_idx,data[bus_recv_idx], error[bus_recv_idx], col[bus_recv_idx] ,bus_rx_data, bus_rx_error);
                                CDEFAULT
                                *berror = false;
                        }
                        if(bus_rx_data != data[bus_recv_idx]){
                                CRED
                                printf("ERROR Data Data @idx %i send: 0x%02X Error %i Col %i Data Received: 0x%02X Error: %i \n", bus_recv_idx,data[bus_recv_idx], error[bus_recv_idx], col[bus_recv_idx] ,bus_rx_data, bus_rx_error);
                                CDEFAULT
                                *berror = false;
                        }
                        bus_recv_idx++;
                        bus_rx_error = false;
                }
        }

        uint16_t speed_delay = 800;     // 800 Ticks at 10MB/s
        if(tb->speed == 1){
                speed_delay = 80;
        }

        for(uint16_t i = 0; i<speed_delay; i++){
                tb->tick();
                if(bus->get_rx_data(&bus_rx_data, &bus_rx_error)){
                        //printf(" Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_tx_data, (uint8_t) phy_tx_error, (uint8_t) phy_tx_nibbleend);
                        if(bus_rx_error != error[bus_recv_idx]){
                                CRED
                                printf("ERROR Bit Data @idx %i send: 0x%02X Error %i Col %i Data Received: 0x%02X Error: %i \n", bus_recv_idx, data[bus_recv_idx], error[bus_recv_idx], col[bus_recv_idx] ,bus_rx_data, bus_rx_error);
                                CDEFAULT
                                *berror = false;
                        }
                        if(bus_rx_data != data[bus_recv_idx]){
                                CRED
                                printf("ERROR Data Data @idx %i send: 0x%02X Error %i Col %i Data Received: 0x%02X Error: %i \n", bus_recv_idx,data[bus_recv_idx], error[bus_recv_idx], col[bus_recv_idx] ,bus_rx_data, bus_rx_error);
                                CDEFAULT
                                *berror = false;
                        }
                        bus_recv_idx++;
                        bus_rx_error = false;
                }
        }
        phy->reset_rx_idx();
}


