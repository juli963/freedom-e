#include "testbench.h"

class	LFSR_TB : public TESTBENCH<VFibonacciLFSR> {
	public:
	
	void init();
	void shift_HW();
	uint64_t taps_to_polynom(const uint8_t* taps, uint8_t num_taps);
	void shift_SW(uint64_t polynom, uint64_t mask, uint64_t* var);
	uint64_t get_HW();

    virtual void tick(void) {
		// Request that the testbench toggle the clock within
		// Verilator
		TESTBENCH<VFibonacciLFSR>::tick();
	}

	private:

};