
#include "../obj_dir/VWatchdogArray.h"
#include "watchdogreg.h"
#include "config.h"
#include "testroutine.h"



//g++ -I obj_dir -I/usr/share/verilator/include VWatchdogArray.cpp VWatchdogArray__Trace.cpp VWatchdogArray__Trace__Slow.cpp VWatchdogArray__Syms.cpp usr/watchdogreg.cpp usr/testbench.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
//make -f VWatchdogArray.mk   

int main(int argc, char **argv) {
	std::srand(static_cast<int>(time(0)));
	Verilated::commandArgs(argc, argv);
	//TESTBENCH<VWatchdogTimer> *tb = new TESTBENCH<VWatchdogTimer>();
	Watchdog_TB *tb = new Watchdog_TB();
	TestRoutines Routines;

	tb->opentrace("trace.vcd");

	printf("Start\n");
	tb->reset();
	tb->init();	// Initialize all


	// Check Inversion
	Routines.TestInversion(tb);

	// Dogs Check
	for(uint8_t i = 0; i < WD_Dogs; i++){	// Dog
		for(uint8_t x = 0; x < WD_Resets; x++){	// Output
			Routines.TestSingleWD(tb,i,x);	// Has Interrupt??
		}
	}

	for(uint8_t i = 0; i<50; i++){
		tb->tick();
	}
	
	printf("--------------------------------------------\n");
	printf("\n");
	
	if (Routines.glerror){
		printf("\033[0;31m");	// Red
		printf("\e[1m");		// Bold Text
		printf("!!!! Test Finished with Failures !!!!\n");
	}else{
		printf("\033[0;32m");	// Green
		printf("\e[1m");		// Bold Text
		printf("!!!! Test Finished successful !!!!\n");
	}
	printf("\033[0;0m");
	printf("\e[0m");
	
	tb->closetrace();
	/*while(!tb->done()) {
		
	} exit(EXIT_SUCCESS);*/
}

