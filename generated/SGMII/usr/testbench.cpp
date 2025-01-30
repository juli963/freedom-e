
#include "Console_Defines.h"
#include "SGMII.h"


#if Datawidth == 8
        #define PHY_Bustype uint8_t
#elif Datawidth == 16
        #define PHY_Bustype uint16_t
#elif Datawidth == 32
        #define PHY_Bustype uint32_t
#elif Datawidth == 64 
        #define PHY_Bustype uint64_t
#endif

#include "net/bus/PHY_Bus2.h"
#include "net/phy/SGMII_PHY.h"


//#include "net/phy/PHY.h"

#include <ctime>
#include <cstdlib>
using namespace std;



uint32_t create_rand(uint32_t min, uint32_t max);

void bus_to_phy( SGMII_TB* tb, PHY *phy, PHY_Bus2 *bus, bool* berror );
void phy_to_bus( SGMII_TB* tb, PHY *phy, PHY_Bus2 *bus, bool* berror );

int main(int argc, char **argv) {
        srand(time(0));  // Initialize random number generator.  
       // printf("Sizeof %i, DWidth %i\n", sizeof(PHY_Bustype), Datawidth);
        bool error_n = true;

        SGMII_TB *tb = new SGMII_TB();
        #if TRACE==1
                tb->opentrace("trace.vcd");
        #endif
        PHY_Bus2::tx_signals bus_signals_tx = {
                .valid = &tb->m_core->io_Bus_tx_valid ,
                .strb = &tb->m_core->io_Bus_tx_bits_strb ,
                .bits = (PHY_Bustype*)&tb->m_core->io_Bus_tx_bits_bits,
                .err = &tb->m_core->io_Bus_tx_bits_err
        };
        PHY_Bus2::rx_signals bus_signals_rx = {
                .valid = &tb->m_core->io_Bus_rx_valid ,
                .strb = &tb->m_core->io_Bus_rx_bits_strb ,
                .bits = (PHY_Bustype*)&tb->m_core->io_Bus_rx_bits_bits,
                .err = &tb->m_core->io_Bus_rx_bits_err
        };

        SGMII_PHY::rx_signals phy_signals_rx = {
                .RXCLOCK = &tb->m_core->io_intf_PCLK,
                .RXDATA = (PHY_Bustype*)&tb->m_core->io_intf_rxdata, 
                .RXDATAK = &tb->m_core->io_intf_rxdatak
        };

        SGMII_PHY::tx_signals phy_signals_tx = {
                .TXCLOCK = &tb->m_core->io_intf_PCLK,
                .TXDATA = (PHY_Bustype*)&tb->m_core->io_intf_txdata, 
                .TXDATAK = &tb->m_core->io_intf_txdatak
        };



        PHY_Bus2 *bus = new PHY_Bus2(&tb->m_core->io_Bus_tx_clock, &tb->m_core->io_Bus_rx_clock, bus_signals_tx, bus_signals_rx, &tb->m_core->io_Bus_crs);
        SGMII_PHY *phy = new SGMII_PHY(phy_signals_tx, phy_signals_rx);

        tb->init();
        printf("Datawidth %i \n", Datawidth);
        
        tb->set_speed(2);
        tb->set_link(1);
        tb->m_core->io_intf_RXValid = 1;

        // Standard Transaction 
        printf("Bus to Phy 1\n");
        bus_to_phy( tb, phy, bus, &error_n );
        printf("Phy to Bus 1\n");
        phy_to_bus( tb, phy, bus, &error_n );
        
/*
        printf("Bus to Phy 2\n");
        bus_to_phy( tb, phy, bus, &error_n );
        printf("Phy to Bus 2\n");
        phy_to_bus( tb, phy, bus, &error_n );

        // Speed Change 
        printf("Change Speed to 100MB/s\n");
        //tb->set_speed(1);
        printf("Bus to Phy 3\n");
        bus_to_phy( tb, phy, bus, &error_n );
        printf("Phy to Bus 3\n");
        phy_to_bus( tb, phy, bus, &error_n );
        printf("Bus to Phy 4\n");
        bus_to_phy( tb, phy, bus, &error_n );
        printf("Phy to Bus 4\n");
        phy_to_bus( tb, phy, bus, &error_n );

        // Multi Transaction 100x?
        printf("Phy to Bus 5\n");
        for(uint8_t i = 0; i<100; i++){
                phy_to_bus( tb, phy, bus, &error_n );
        }
        printf("Bus to Phy 5\n");
        for(uint8_t i = 0; i<100; i++){
                bus_to_phy( tb, phy, bus, &error_n );
        }
        printf("Toggling Directions\n");
        for(uint8_t i = 0; i<100; i++){
                bus_to_phy( tb, phy, bus, &error_n );
                phy_to_bus( tb, phy, bus, &error_n );
        }
*/

        // 1 for no error, 0 for error
        return error_n;
}

uint32_t create_rand(uint32_t min, uint32_t max){       // Max nur bis, nicht einschließlich
        return min+ (rand() % max);
}


void bus_to_phy( SGMII_TB* tb, PHY* phy, PHY_Bus2* bus, bool* berror ){
        
        uint16_t length = create_rand(32, 512);

        uint8_t data[length] = {0};
        uint8_t error[length] = {0};

        uint8_t phy_tx_data[Datawidth/8] = {0};
        uint16_t phy_recv_idx=0;
        bool phy_tx_error[Datawidth/8] = {false};
        bool phy_tx_nibbleend[Datawidth/8] = {false};

        uint8_t temp;

        for(uint16_t i = 0; i<length;i++){
                data[i] = create_rand(0, 256);
                //error[i] = create_rand(0, 2);
        }
        
        bus->send_tx_data(data,error,length);
        while( bus->send_tx_data(data,error,length) ){
                tb->tick();
                
                temp = phy->get_tx_data(phy_tx_data, phy_tx_error, phy_tx_nibbleend);
                if(temp>0){
                        for(uint8_t i = 0; i<temp; i++){
                                //printf(" Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_tx_data, (uint8_t) phy_tx_error, (uint8_t) phy_tx_nibbleend);
                                if(phy_tx_error[i] != error[phy_recv_idx]){
                                        CRED
                                        printf("ERROR Bit @idx %i Data send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data[i], phy_tx_error[i], phy_tx_nibbleend[i]);
                                        CDEFAULT
                                        *berror = false;
                                }
                                if(phy_tx_data[i] != data[phy_recv_idx]){
                                        CRED
                                        printf("ERROR Data Data @idx %i send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data[i], phy_tx_error[i], phy_tx_nibbleend[i]);
                                        CDEFAULT
                                        *berror = false;
                                }
                                if(phy_tx_nibbleend[i]){
                                        CRED
                                        printf("ERROR Nibble @idx %i Data send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data[i], phy_tx_error[i], phy_tx_nibbleend[i]);
                                        CDEFAULT
                                        *berror = false;
                                }
                                phy_recv_idx++;
                                phy_tx_error[i] = false;
                        }
                }
        }

        uint16_t speed_delay = 800;     // 800 Ticks at 10MB/s
        if(tb->speed == 1){
                speed_delay = 80;
        }

        for(uint16_t i = 0; i<speed_delay; i++){
                tb->tick();
                temp = phy->get_tx_data(phy_tx_data, phy_tx_error, phy_tx_nibbleend);
                if(temp>0){
                        for(uint8_t i = 0; i<temp; i++){
                                //printf(" Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_tx_data, (uint8_t) phy_tx_error, (uint8_t) phy_tx_nibbleend);
                                if(phy_tx_error[i] != error[phy_recv_idx]){
                                        CRED
                                        printf("ERROR Bit @idx %i Data send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data[i], phy_tx_error[i], phy_tx_nibbleend[i]);
                                        CDEFAULT
                                        *berror = false;
                                }
                                if(phy_tx_data[i] != data[phy_recv_idx]){
                                        CRED
                                        printf("ERROR Data Data @idx %i send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data[i], phy_tx_error[i], phy_tx_nibbleend[i]);
                                        CDEFAULT
                                        *berror = false;
                                }
                                if(phy_tx_nibbleend[i]){
                                        CRED
                                        printf("ERROR Nibble @idx %i Data send: 0x%02X Error %i Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_recv_idx,data[phy_recv_idx], error[phy_recv_idx] ,phy_tx_data[i], phy_tx_error[i], phy_tx_nibbleend[i]);
                                        CDEFAULT
                                        *berror = false;
                                }
                                phy_recv_idx++;
                                phy_tx_error[i] = false;
                        }
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

void phy_to_bus( SGMII_TB* tb, PHY* phy, PHY_Bus2* bus, bool* berror){
        
        uint16_t length = create_rand(32, 512);
        uint16_t start = create_rand(1,16);
        //uint16_t stop = create_rand(1, 16);
        //stop = length-stop;
//printf("Length %i , Start= %i, Stop= %i", length, start, stop);
        uint8_t data[length] = {0};
        bool error[length] = {false};
        bool col[length] = {false};

        uint8_t bus_rx_data[Datawidth/8];
        uint16_t bus_recv_idx=0;
        uint8_t bus_rx_error[Datawidth/8];
        uint8_t temp = 0;

        for(uint16_t i = 0; i<length;i++){
                data[i] = create_rand(0, 256);
                //error[i] = create_rand(0, 2) > 0;
        }

        for(uint16_t i = start; i<length;i++){
                col[i] = true;
        }
        
        //phy->send_rx_data(data,error, col,length);
        while( phy->send_rx_data(data,error, col,length) ){
                tb->tick();

                temp = bus->get_rx_data(bus_rx_data, bus_rx_error);
                if(temp>0){
                        for(uint8_t i = 0; i<temp; i++){  
                                //printf(" Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_tx_data, (uint8_t) phy_tx_error, (uint8_t) phy_tx_nibbleend);
                                if((bool)bus_rx_error[i] != error[bus_recv_idx+start]){
                                        CRED
                                        printf("ERROR Bit Data @idx %i send: 0x%02X Error %i Col %i Data Received: 0x%02X Error: %i \n", bus_recv_idx,data[bus_recv_idx+start], error[bus_recv_idx+start], col[bus_recv_idx+start] ,bus_rx_data[i], bus_rx_error[i]);
                                        CDEFAULT
                                        *berror = false;
                                }
                                if(bus_rx_data[i] != data[bus_recv_idx+start]){
                                        CRED
                                        printf("ERROR Data Data @idx %i send: 0x%02X Error %i Col %i Data Received: 0x%02X Error: %i \n", bus_recv_idx,data[bus_recv_idx+start], error[bus_recv_idx+start], col[bus_recv_idx+start] ,bus_rx_data[i], bus_rx_error[i]);
                                        CDEFAULT
                                        *berror = false;
                                }
                                bus_recv_idx++;
                                bus_rx_error[i] = false;
                        }
                }
        }

        uint16_t speed_delay = 800;     // 800 Ticks at 10MB/s
        if(tb->speed == 1){
                speed_delay = 80;
        }

        for(uint16_t i = 0; i<speed_delay; i++){
                tb->tick();
                temp = bus->get_rx_data(bus_rx_data, bus_rx_error);
                if(temp>0){
                        for(uint8_t i = 0; i<temp; i++){  
                                //printf(" Data Received: 0x%02X Error: %i, Nibbleend: %i \n", phy_tx_data, (uint8_t) phy_tx_error, (uint8_t) phy_tx_nibbleend);
                                if((bool)bus_rx_error[i] != error[bus_recv_idx+start]){
                                        CRED
                                        printf("ERROR Bit Data @idx %i send: 0x%02X Error %i Col %i Data Received: 0x%02X Error: %i \n", bus_recv_idx,data[bus_recv_idx+start], error[bus_recv_idx+start], col[bus_recv_idx+start] ,bus_rx_data[i], bus_rx_error[i]);
                                        CDEFAULT
                                        *berror = false;
                                }
                                if(bus_rx_data[i] != data[bus_recv_idx+start]){
                                        CRED
                                        printf("ERROR Data Data @idx %i send: 0x%02X Error %i Col %i Data Received: 0x%02X Error: %i \n", bus_recv_idx,data[bus_recv_idx+start], error[bus_recv_idx+start], col[bus_recv_idx+start] ,bus_rx_data[i], bus_rx_error[i]);
                                        CDEFAULT
                                        *berror = false;
                                }
                                bus_recv_idx++;
                                bus_rx_error[i] = false;
                        }
                }
        }

        if(bus_recv_idx != length-start){
                CRED
                printf("Not all Data received bus_receive %i , Length %i \n", bus_recv_idx, length-start);
                CDEFAULT
                *berror = false;
                
        }

        phy->reset_rx_idx();
}


 /*
        val intf = Flipped(     
                                val TXDetectRX = Input(Bool())
                                val TXElecIdle = Input(Bool())
                                val TXCompliance = Input(Bool())
 
                                val RXPolarity = Input(Bool())

                                val resetn = Input(Bool())
                                val Powerdown = Input(UInt(2.W))
                                val Rate = Input(UInt(RateWidth.W))    // 0 -> 2.5GT/s 1-> 5.0GT/s
                                val TXDeemph = Input(UInt(2.W))    // 0 -> -6dB 1-> -3.5dB 2-> 0dB
                                val TXMargin = Input(UInt(3.W))    // See table for voltage levels
                                val TXSwing = Input(Bool())    // 0 -> Full Swing 1 -> Low Swing


                                // Status
                                val RXValid = Output(Bool())
                                val PhyStatus = Output(Bool())
                                val RXElecIdle = Output(Bool())
                                val RXStatus = Output(UInt(3.W))


        val stats = Flipped(    val Link = Output(Bool())
                                val Speed = Output(UInt(4.W))   // 0->10M 1->100M 2-> 1G 3->2.5G 4->5G 5->10G 6->25G 7->40G 8->100G
                                val Duplex = Output(Bool())   // True->Full False->Half)
        
        val Bus =               val tx_clock = Output(Clock())
                                val rx_clock = Output(Clock())
                                val tx = Input(new ValidIO(new Bundle{
                                        val strb = UInt( (DataWidth/8).W )
                                        val bits = UInt(DataWidth.W)
                                        val err = UInt((DataWidth/8).W)
                                        }))    // tx.valid and tx.bits
                                val rx = Output(new ValidIO(new Bundle{
                                        val strb = UInt( (DataWidth/8).W )
                                        val bits = UInt(DataWidth.W)
                                        val err = UInt((DataWidth/8).W)
                                        }))
                                val crs = Output(Bool())

        val config = new Bundle{
            val Powerdown = Input(UInt(2.W))
            val TXElecIdle = Input(Bool())
            val TXDeemph = Input(UInt(2.W))    // 0 -> -6dB 1-> -3.5dB 2-> 0dB
            val TXMargin = Input(UInt(3.W))    // See table for voltage levels
            val TXSwing = Input(Bool())    // 0 -> Full Swing 1 -> Low Swing
            val RXElecIdle = Output(Bool())
            val Rate = Input(UInt(RateWidth.W))

            val negSpeed = Output(UInt(3.W))
        */