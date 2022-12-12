
#include <cstddef>
#include <verilated_vcd_c.h>

#if !defined(TESTBENCH_H)
        #define TESTBENCH_H 1

        template<class ETHClockULE>        class TESTBENCH {
                public:
                VerilatedVcdC        *m_trace;
                unsigned long        m_tickcount;
                ETHClockULE        *m_core;
                uint8_t out_ff = 0;
                uint8_t changed = 0;
                TESTBENCH(void) {
                        m_core = new ETHClockULE;
                        m_tickcount = 0;
                        Verilated::traceEverOn(true);
                }

                virtual        void opentrace(const char *vcdname) {
                        if (!m_trace) {
                                m_trace = new VerilatedVcdC;
                                m_core->trace(m_trace, 99);
                                m_trace->open(vcdname);
                        }
                }

                virtual void closetrace(void) {
                        if (m_trace) {
                                m_trace->close();
                                m_trace = NULL;
                        }
                }

                virtual ~TESTBENCH(void) {
                        delete m_core;
                        m_core = NULL;
                }

                virtual void reset(void) {
                        m_core->reset = 1;
                        // Make sure any inheritance gets applied
                        this->tick();
                        m_core->reset = 0;

                }

                virtual void        tick(void) {
                        changed = 0;
                        // Increment our own internal time reference
                        m_tickcount++;

                        // Make sure any combinatorial logic depending upon
                        // inputs that may have changed before we called tick()
                        // has settled before the rising edge of the clock.
                        m_core->clock = 0;
                        out_ff = m_core->io_outclock;
                        m_core->eval();

                        // Toggle the clock
                        if(m_trace) m_trace->dump(10*m_tickcount-2);

                        // Rising edge
                        m_core->clock = 1;
                        
                        if( out_ff !=  m_core->io_outclock ){
                                changed++;
                                //changed = true;
                                //printf("Changed\n");
                        }

                        out_ff = m_core->io_outclock;
                        m_core->eval();
                        if(m_trace) m_trace->dump(10*m_tickcount);

                        if( out_ff !=  m_core->io_outclock ){
                                changed++;
                                //changed = true;
                                //printf("Changed\n");
                        }
                        out_ff = m_core->io_outclock;
                        // Falling edge
                        m_core->clock = 0;
                        m_core->eval();

                        if( out_ff !=  m_core->io_outclock ){
                                changed++;
                                //changed = true;
                                //printf("Changed\n");
                        }

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

                virtual bool        done(void) { return (Verilated::gotFinish()); }
        };
#endif
