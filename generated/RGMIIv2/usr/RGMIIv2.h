
#include "testbench.h"
#include "../obj_dir/VRGMIIv2.h"
#include "stdint.h"

#if !defined(RGMIIv2_H)
    #define RGMIIv2_H 1
    class        RGMIIv2_TB : public TESTBENCH<VRGMIIv2> {
        public:
            void init();
            void change_speed(uint8_t speed);

            void tick(void) {
                // Increment our own internal time reference
                m_tickcount++;

                // Make sure any combinatorial logic depending upon
                // inputs that may have changed before we called tick()
                // has settled before the rising edge of the clock.
                m_core->clock = 0;
                m_core->eval();

                // Toggle the clock
                if(m_trace) m_trace->dump(10*m_tickcount-2);

                // Rising edge
                m_core->clock = 1;
                
                refcounter++;
                rx_counter++;
                

                if(speed == 1 && rx_counter >= 10){
                    m_core->io_intf_rx_RXC ^= 1;
                    //tx_clock ^= 1;
                    rx_counter = 0;
                }else if(speed == 0 && rx_counter >= 100){
                    m_core->io_intf_rx_RXC ^= 1;
                    //tx_clock ^= 1;
                    rx_counter = 0;
                }else if(speed == 2){
                    m_core->io_intf_rx_RXC ^= 1;
                   // tx_clock ^= 1;
                    //m_core->io_crs_in = tx_clock;
                    rx_counter = 0;
                }
                

                if(refcounter >= 2){
                    m_core->io_refclock ^= 0x1;
                    refcounter = 0;
                    if(m_core->io_refclock >= 0x1){
                        tx_temp = m_core->io_intf_tx_TXC;
                        //printf("Load TX Temp 0x%02X\n", tx_temp);
                    }   
                }
                tx_clock = tx_temp & 0x01;
                //m_core->io_crs_in = tx_clock;
                tx_temp >>= 1;


                m_core->eval();
                if(m_trace) m_trace->dump(10*m_tickcount);

                // Falling edge
                m_core->clock = 0;
                m_core->eval();

                if (m_trace) {
                    // This portion, though, is a touch different.
                    // After dumping our values as they exist on the
                    // negative clock edge ...
                    m_trace->dump(10*m_tickcount+5);
                    //
                    // We'll also need to make sure we flush any I/O to
                    // the trace file, so that we can use the assert()
                    // function between now and the next tick if we want to.
                    m_trace->flush();
                }

            }

            uint8_t tx_clock = 0;
            uint8_t speed = 0;

        private:
            uint8_t counter = 0;
            uint8_t refcounter = 0;
            uint16_t rx_counter = 0;
            uint8_t tx_temp = 0;

    };
#endif
