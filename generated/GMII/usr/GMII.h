
#include "testbench.h"
#include "../obj_dir/VGMII.h"
#include "stdint.h"

#if !defined(GMII_H)
    #define GMII_H 1
    class        GMII_TB : public TESTBENCH<VGMII> {
        public:
            void init();
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
                counter++;
                refcounter++;
                if(counter >= 5 && speed == 1){
                    m_core->io_intf_rx_RXCLK ^= 0x1;
                    m_core->io_intf_tx_TXCLK ^= 0x1;
                    counter = 0;
                }else if(counter >= 50 && speed == 0){
                    m_core->io_intf_rx_RXCLK ^= 0x1;
                    m_core->io_intf_tx_TXCLK ^= 0x1;
                    counter = 0;
                }else if(counter >= 1 && speed > 1){
                    m_core->io_intf_rx_RXCLK ^= 0x1;
                    m_core->io_intf_tx_TXCLK ^= 0x1;
                    
                    counter = 0;
                }

                if(refcounter >= 1){
                    m_core->io_refclock ^= 0x1;
                    refcounter = 0;
                }

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
            void change_speed(uint8_t speed);
            uint8_t speed = 0;  // Let's say base frequency is 100MHz
        private:
            uint8_t counter = 0;
            uint8_t refcounter = 0;


    };
#endif
