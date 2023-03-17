mod_h = """
#include "testbench.h"
#include "../obj_dir/VMOD.h"
#include "stdint.h"

#if !defined(MOD_H)
    #define MOD_H 1
    class        MOD_TB : public TESTBENCH<VMOD> {
        public:
            void init();
        private:

    };
#endif
"""

mod_c = """
#include "MOD.h"

void MOD_TB::init(){
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}
"""

tb_h ="""
#include <cstddef>
#include <verilated_vcd_c.h>

#if !defined(TESTBENCH_H)
        #define TESTBENCH_H 1

        template<class MODULE>        class TESTBENCH {
                public:
                VerilatedVcdC        *m_trace;
                unsigned long        m_tickcount;
                MODULE        *m_core;

                TESTBENCH(void) {
                        m_core = new MODULE;
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

                virtual bool        done(void) { return (Verilated::gotFinish()); }
        };
#endif
"""

tb_c = """
#include "Console_Defines.h"
#include "MOD.h"

int main(int argc, char **argv) {

        MOD_TB *tb = new MOD_TB();
        #if TRACE==1
                tb->opentrace("trace.vcd");
        #endif
        tb->init();
}
"""

console_defs = """
#define CRED printf("\\033[31m");
#define CGREEN printf("\\033[32m");
#define CDEFAULT printf("\\033[39m");
"""