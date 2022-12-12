
#include "testbench.h"
#include "../obj_dir/VMII.h"
#include "stdint.h"

#if !defined(MII_H)
    #define MII_H 1
    class        MII_TB : public TESTBENCH<VMII> {
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
                if(counter >= 2 && speed == 1){
                    m_core->io_intf_rx_RXCLK ^= 0x1;
                    m_core->io_intf_tx_TXCLK ^= 0x1;
                    counter = 0;
                }else if(counter >= 20 && speed == 0){
                    m_core->io_intf_rx_RXCLK ^= 0x1;
                    m_core->io_intf_tx_TXCLK ^= 0x1;
                    counter = 0;
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

    };
#endif
