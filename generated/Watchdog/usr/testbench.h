#include <cstddef>
#include "../obj_dir/VWatchdogTimer.h"
#include <verilated_vcd_c.h>
//g++ -I obj_dir -I/usr/share/verilator/include VWatchdogTimer__Syms.cpp VWatchdogTimer.cpp usr/testbench.cpp usr/watchdogreg.cpp /usr/share/verilator/include/verilated.cpp -o usr/testbench.o

template<class MODULE>	class TESTBENCH {
	public: 
	VerilatedVcdC	*m_trace;
	unsigned long	m_tickcount;
	MODULE	*m_core;

	TESTBENCH(void) {
		m_core = new MODULE;
		m_tickcount = 0;
		Verilated::traceEverOn(true);
	}

	virtual	void opentrace(const char *vcdname) {
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
		m_core->io_corerst = 1;
		// Make sure any inheritance gets applied
		this->tick();
		m_core->reset = 0;
		m_core->io_corerst = 0;
	}

	virtual void tick(void) {
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

	virtual bool done(void) { return (Verilated::gotFinish()); }
};