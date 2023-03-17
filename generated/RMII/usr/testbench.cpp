
#include "Console_Defines.h"
#include "RMII.h"
#include "RMII_PHY.h"
#include "PHY_Bus2.h"

#include <ctime>

#include <cstdlib>

using namespace std;

uint32_t create_rand(uint32_t min, uint32_t max);

void bus_to_phy( RMII_TB* tb, RMII_PHY *phy, PHY_Bus2 *bus, bool* berror );
void phy_to_bus( RMII_TB* tb, RMII_PHY *phy, PHY_Bus2 *bus, bool* berror, bool with_Col, bool with_crs );

int main(int argc, char **argv) {
        srand(time(0));  // Initialize random number generator.  

        bool error_n = true;
/*
class RMII_Interface() extends Bundle { // 50 MHz
  val REF_CLK = Output(Clock())
  val tx = new Bundle{
      val TXEN = Output(Bool())   
      val TXD = Output(UInt(2.W))
  }
  val rx = new Bundle{
      val CRS_DV = Input(Bool())
      val RXER = Input(Bool())
      val COL = Input(Bool())
      val RXD = Input(UInt(2.W))
  }
}
*/

        RMII_TB *tb = new RMII_TB();

        RMII_PHY::rx_signals phy_signals_rx = {
                .RXCLK = &tb->m_core->io_intf_REF_CLK,
                .RXDV = &tb->m_core->io_intf_rx_CRS_DV, 
                .RXER = &tb->m_core->io_intf_rx_RXER, 
                .RXD = &tb->m_core->io_intf_rx_RXD,
                .COL = &tb->m_core->io_intf_rx_COL
        };

        RMII_PHY::tx_signals phy_signals_tx = {
                .TXCLK = &tb->m_core->io_intf_REF_CLK,
                .TXEN = &tb->m_core->io_intf_tx_TXEN, 
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

        RMII_PHY *phy = new RMII_PHY(phy_signals_tx, phy_signals_rx, &tb->speed);
        PHY_Bus2 *bus = new PHY_Bus2(&tb->m_core->io_Bus_tx_clock, &tb->m_core->io_Bus_rx_clock, bus_signals_tx, bus_signals_rx, &tb->m_core->io_Bus_crs);

        #if TRACE == 1
                tb->opentrace("trace.vcd");
        #endif
        tb->init();
        phy->init();
        bus->init();
        tb->change_speed(0);
/*
        bus->reset_tx_idx();
        phy->reset_rx_idx();

        uint8_t tx_data[8] =    {0x55,0xFA,0xAA,0xAB,0x55,0x54,0x69,0x87};
        bool tx_error[8] =      {false,false,true,false,false,true,false,false};
        bool tx_col[8] =        {false,true,false,false,true,false,false,false};

        uint8_t phy_tx_data = 0;
        bool phy_tx_error = false;
        bool phy_tx_nibbleend = false;
        //bool RMII_PHY::get_tx_data(uint8_t* data, bool* error, bool* nibbleend)
        //while( bus->send_tx_data(tx_data,tx_error,8) ){
        //send_rx_data(uint8_t* data, bool* error, bool* col, uint16_t len);    // return is busy state
        while( phy->send_rx_data(tx_data,tx_error, tx_col,8) ){
                tb->tick();
                //if(phy->get_tx_data(&phy_tx_data, &phy_tx_error, &phy_tx_nibbleend)){
                if(bus->get_rx_data(&phy_tx_data, &phy_tx_error)){
                        printf(" Data Received: 0x%02X Error: %i \n", phy_tx_data, (uint8_t) phy_tx_error);
                        phy_tx_error = false;
                }
        }



        for(uint16_t i = 0; i<1024; i++){
                tb->tick();
                //if(phy->get_tx_data(&phy_tx_data, &phy_tx_error, &phy_tx_nibbleend)){
                if(bus->get_rx_data(&phy_tx_data, &phy_tx_error)){
                        printf(" Data Received: 0x%02X Error: %i \n", phy_tx_data, (uint8_t) phy_tx_error);
                        phy_tx_error = false;
                }
        }
*/



                // Standard Transaction 
                printf("Bus to Phy 1\n");
                bus_to_phy( tb, phy, bus, &error_n );
                printf("Phy to Bus 1\n");
                phy_to_bus( tb, phy, bus, &error_n, true, false );
               

                // Check CRS 
               if(tb->m_core->io_Bus_crs > 0){
                        CRED
                        printf("ERR_CRS CRS not low. \n");
                        CDEFAULT
                        error_n = false;
                }
              printf("Bus to Phy 2\n");
                bus_to_phy( tb, phy, bus, &error_n );
                printf("Phy to Bus 2\n");
                phy_to_bus( tb, phy, bus, &error_n, true, true );
                if(tb->m_core->io_Bus_crs == 0){
                        CRED
                        printf("ERR_CRS CRS not going high. \n");
                        CDEFAULT
                        error_n = false;
                }

                // Speed Change 
                printf("Change Speed to 100MB/s\n");
                tb->change_speed(1);
                printf("Bus to Phy 3\n");
                bus_to_phy( tb, phy, bus, &error_n );
                printf("Phy to Bus 3\n");
                phy_to_bus( tb, phy, bus, &error_n, true, false );
                
                // Check CRS 
                if(tb->m_core->io_Bus_crs > 0){
                        CRED
                        printf("ERR_CRS CRS not low. \n");
                        CDEFAULT
                        error_n = false;
                }
                printf("Bus to Phy 4\n");
                bus_to_phy( tb, phy, bus, &error_n );
                printf("Phy to Bus 4\n");
                phy_to_bus( tb, phy, bus, &error_n, true, true );
                if(tb->m_core->io_Bus_crs == 0){
                        CRED
                        printf("ERR_CRS CRS not going high. \n");
                        CDEFAULT
                        error_n = false;
                }
                // Multi Transaction 100x?
                printf("Phy to Bus 5\n");
                for(uint8_t i = 0; i<100; i++){
                        phy_to_bus( tb, phy, bus, &error_n, true, true );
                }
                printf("Bus to Phy 5\n");
                for(uint8_t i = 0; i<100; i++){
                        bus_to_phy( tb, phy, bus, &error_n );
                }
                printf("Toggling Directions\n");
                for(uint8_t i = 0; i<100; i++){
                        bus_to_phy( tb, phy, bus, &error_n );
                        phy_to_bus( tb, phy, bus, &error_n, true, false );
                }
        

        return error_n;       
}

uint32_t create_rand(uint32_t min, uint32_t max){       // Max nur bis, nicht einschließlich
        return min+ (rand() % max);
}

void bus_to_phy( RMII_TB* tb, RMII_PHY* phy, PHY_Bus2* bus, bool* berror ){
        
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
                //printf("Data 0x%02X \n", data[i]);
        }
        while(error[phy_recv_idx]){
                phy_recv_idx++;   
        }


        //bus->send_tx_data(data,error,length);
        while( bus->send_tx_data(data,error,length) ){
                tb->tick();
                if(phy->get_tx_data(&phy_tx_data, &phy_tx_error, &phy_tx_nibbleend)){
                        //printf("@idx %i Data send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data, phy_tx_error, phy_tx_nibbleend);

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
                        while(error[phy_recv_idx] && phy_recv_idx < length){
                             phy_recv_idx++;   
                        }
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
                        while(error[phy_recv_idx] && phy_recv_idx < length){
                             phy_recv_idx++;   
                        }
                        phy_tx_error = false;
                }
        }



        if(phy_recv_idx != length){
                CRED
                printf("Not all Data received phy_receive %i , Length %i \n", phy_recv_idx, length);
                CDEFAULT
                *berror = false;
                
        }

        bus->reset_tx_idx();
}

void phy_to_bus( RMII_TB* tb, RMII_PHY* phy, PHY_Bus2* bus, bool* berror, bool with_Col, bool with_crs ){
        
        uint16_t length = create_rand(32, 512);

        uint8_t data[length] = {0x0};
        bool error[length] = {false};
        bool col[length] = {false};

        uint8_t bus_rx_data;
        uint16_t bus_recv_idx=0;
        bool bus_rx_error;
        data[0] = 0x55; // We need to synchronize
        for(uint16_t i = 1; i<length;i++){
                data[i] = create_rand(0, 256);
                error[i] = create_rand(0, 2) > 0;
                col[i] = create_rand(0, 2) > 0;
        }
        
        //printf("data: 0x%02X\n", data[0]);
        
        //phy->send_rx_data(data,error, col,length);
        while( phy->send_rx_data(data, error, col, with_crs, length) ){
               // printf("data: 0x%02X\n", data[0]);
                tb->tick();
                if(bus->get_rx_data(&bus_rx_data, &bus_rx_error)){
                        //printf(" Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_tx_data, (uint8_t) phy_tx_error, (uint8_t) phy_tx_nibbleend);
                        if( bus_rx_error != error[bus_recv_idx] && bus_rx_error != col[bus_recv_idx] ){
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
                        if(bus_rx_error != error[bus_recv_idx] && bus_rx_error != col[bus_recv_idx] ){
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

        /*uint16_t count = 0;
        for(uint16_t i = 0; i<length; i++){
                if(!error[i] && !col[i]){
                      count++;  
                }
        }*/

        if(bus_recv_idx != length){
                CRED
                printf("Not all Data received bus_receive %i , Length %i \n", bus_recv_idx, length);
                CDEFAULT
                *berror = false;
                
        }

        phy->reset_rx_idx();
}