
#include "../obj_dir/VWatchdogArray.h"
#include "watchdogreg.h"
#include "config.h"
//g++ -I obj_dir -I/usr/share/verilator/include VWatchdogArray.cpp VWatchdogArray__Trace.cpp VWatchdogArray__Trace__Slow.cpp VWatchdogArray__Syms.cpp usr/watchdogreg.cpp usr/testbench.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
//make -f VWatchdogArray.mk   

int main(int argc, char **argv) {
	Verilated::commandArgs(argc, argv);
	//TESTBENCH<VWatchdogTimer> *tb = new TESTBENCH<VWatchdogTimer>();
	Watchdog_TB *tb = new Watchdog_TB();
	tb->opentrace("trace.vcd");


	tb->reset();
	//tb->setOutput(1,0);
    tb->sWD.cfg.field.deglitch = 0;  // Window Mode
	tb->sWD.cfg.field.scale = 0;
	tb->sWD.cfg.field.encoreawake = 0;  // Start Counter when corerst = 0
	tb->sWD.cfg.field.zerocmp = 0;
	tb->sWD.cfg.field.enalways = 1; // Start Counter
	tb->sWD.cfg.field.en = 1;   // Enable Output
	tb->sWD.compare0 = 15;
	tb->sWD.mux = (1<<0);
	tb->sWD.pulsewidth = 5;

    unsigned long offs = tb->configWatchdog(&tb->sWD,0);	
	
	printf("Starte Test 1 -> enalways, no zerocmp and cmp = 15 and pw = 5 \n");
	while(!(tb->readOutput() > 0)){
		tb->tick();
		//printf("%li \n",(tb->m_tickcount-offs));
	}
    printf("Ende Test 1 Zyklen = %li \n",tb->m_tickcount-offs);
    tb->tick();
    tb->disableWatchdog();

    printf("Starte Test 2 -> encoreawake, zerocmp, scale = 1 and cmp = 15 \n");
    tb->sWD.cfg.field.encoreawake = 1;  // Start Counter when corerst = 0
    tb->sWD.cfg.field.scale = 1;
    tb->sWD.cfg.field.zerocmp = 1;
    tb->sWD.cfg.field.enalways = 0; // Start Counter
	tb->sWD.pulsewidth = 70;
    offs = tb->configWatchdog(&tb->sWD,0);
	
	//tb->sWD.cfg.field.scale = 1;
	//tb->sWD.compare0 = 10;
	//tb->configWatchdog(&tb->sWD,1);

	while(!(tb->readOutput() > 0)){
		tb->tick();
		//printf("%li \n",(tb->m_tickcount-offs));
	}
    printf("Ende Test 2 Zyklen = %li \n",tb->m_tickcount-offs);
	for(int i = 0; i<5000;i++){
		tb->tick();
	}
	printf("Ende Test 2 Zyklen = %li \n",tb->m_tickcount-offs);
    tb->tick();
    tb->disableWatchdog();

    printf("Starte Test 3 -> encoreawake, zerocmp, scale = 0 and cmp0 = 15 and cmp1 = 27 \n");
    tb->sWD.cfg.field.deglitch = 1;  // Window Mode
    tb->sWD.cfg.field.scale = 0;
    tb->sWD.compare0 = 15;
    tb->sWD.compare1 = 27;
    offs = tb->configWatchdog(&tb->sWD,0);
	while(!(tb->readOutput() > 0)){
        if ((tb->m_tickcount-offs) == 20){
            tb->unlock();
            tb->writeReg(Watchdog_TB::reg_ctrl_feed,0xD09F00D);
        }
		tb->tick();
		//printf("%li \n",(tb->m_tickcount-offs));
	}
    printf("Ende Test 3 Zyklen = %li \n",tb->m_tickcount-offs);
    tb->tick();
    tb->disableWatchdog();

    printf("Starte Test 4 -> encoreawake, zerocmp, scale = 0 and cmp0 = 15 and cmp1 = 27 \n");
    tb->sWD.cfg.field.deglitch = 1;  // Window Mode
    tb->sWD.cfg.field.scale = 0;
    tb->sWD.compare0 = 15;
    tb->sWD.compare1 = 27;
    offs = tb->configWatchdog(&tb->sWD,0);
	while(!(tb->readOutput() > 0)){
        if ((tb->m_tickcount-offs) == 10){
            tb->unlock();
            tb->writeReg(Watchdog_TB::reg_ctrl_feed,0xD09F00D);
        }
		tb->tick();
		//printf("%li \n",(tb->m_tickcount-offs));
	}
    printf("Ende Test 4 Zyklen = %li \n",tb->m_tickcount-offs);
    tb->tick();
    tb->disableWatchdog();


	for(int i = 0; i<50; i++){
		tb->tick();
	}

	printf("Finished\n");
	tb->closetrace();
	/*while(!tb->done()) {
		
	} exit(EXIT_SUCCESS);*/
}