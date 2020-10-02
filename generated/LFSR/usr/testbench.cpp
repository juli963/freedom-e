
#include "../obj_dir/VFibonacciLFSR.h"
#include "config.h"
#include "LFSR.h"

int main(int argc, char **argv) {
	Verilated::commandArgs(argc, argv);
	//TESTBENCH<VWatchdogTimer> *tb = new TESTBENCH<VWatchdogTimer>();
	LFSR_TB *tb = new LFSR_TB();
	tb->opentrace("trace.vcd");
	tb->init();
	uint64_t polynom = tb->taps_to_polynom(LFSR_taps,sizeof(LFSR_taps)/sizeof(LFSR_taps[0]));
	uint64_t mask = 0;
	for(uint64_t i = 0; i<LFSR_width; i++){
		mask |= ((uint64_t)1<<i);
	}

	for(uint32_t i = 0; i<LFSR_shifts; i++){
		if(tb->get_HW() != LFSR_init){
			printf("\033[0;31m");	// Red Color
		}else{
			printf("\033[0;32m");	// Green Color
		}
		printf("HW=0x%lX SW=0x%lX \n",tb->get_HW(), LFSR_init);
		tb->shift_HW();
		tb->shift_SW(polynom, mask, &LFSR_init);
	}
	if(tb->get_HW() != LFSR_init){
		printf("\033[0;31m");	// Red Color
	}else{
		printf("\033[0;32m");	// Green Color
	}
	printf("HW=0x%lX SW=0x%lX \n",tb->get_HW(), LFSR_init);
	printf("\033[0;0m");	// Default Color
	tb->closetrace();
}