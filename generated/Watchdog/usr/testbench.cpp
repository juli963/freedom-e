
#include "../obj_dir/VWatchdogTimer.h"
#include "watchdogreg.h"
#include "config.h"
#include <cstdlib>

#define CRED		printf("\033[0;31m");	// Red Color
#define CGREEN		printf("\033[0;32m");	// Green Color
#define CDEFAULT 	printf("\033[0;0m");	// Default Color

int main(int argc, char **argv) {
	std::srand(static_cast<int>(time(0)));
	unsigned long offs = 0;
	bool state = false;
	uint8_t testnumber = 1;
	Verilated::commandArgs(argc, argv);
	Watchdog_TB *tb = new Watchdog_TB();
	tb->opentrace("trace.vcd");
	tb->init();

	uint64_t max_cmp = 0;
	for(uint8_t i = 0; i<WD_cmpWidth; i++){
		max_cmp |= ((uint64_t)1<<i);
	}
	
	tb->unlock();
	tb->writeReg(Watchdog_TB::reg_ctrl_deglitch,0);
#if defined(MODE_WINDOW) 
		if(!tb->readReg(Watchdog_TB::reg_ctrl_deglitch)){
			CRED
			printf("FEHLER Mode Register hat den falschen Wert\n");
			CDEFAULT
		}
#elif defined(MODE_TIMEOUT) || defined(MODE_BOTH)
		if(tb->readReg(Watchdog_TB::reg_ctrl_deglitch)){
			CRED
			printf("FEHLER Mode Register hat den falschen Wert\n");
			CDEFAULT
		}
#endif
	tb->unlock();
	tb->writeReg(Watchdog_TB::reg_ctrl_deglitch,1);
#if defined(MODE_TIMEOUT) 
		if(tb->readReg(Watchdog_TB::reg_ctrl_deglitch)){
			CRED
			printf("FEHLER Mode Register hat den falschen Wert\n");
			CDEFAULT
		}
#elif defined(MODE_WINDOW) || defined(MODE_BOTH)
		if(!tb->readReg(Watchdog_TB::reg_ctrl_deglitch)){
			CRED
			printf("FEHLER Mode Register hat den falschen Wert\n");
			CDEFAULT
		}
#endif


#if defined(MODE_TIMEOUT) || defined(MODE_BOTH)

// Test 1
    tb->sWD.cfg.field.deglitch = 0;  // Timeout Mode
    tb->sWD.cfg.field.interrupt = 0;  // Timeout Mode
	tb->sWD.cfg.field.scale = 0;
	tb->sWD.cfg.field.encoreawake = 0;  // Start Counter when corerst = 0
	tb->sWD.cfg.field.zerocmp = 0;
	tb->sWD.cfg.field.enalways = 1; // Start Counter
	tb->sWD.cfg.field.en = 1;   // Enable Output
	tb->sWD.compare0 = std::rand() % max_cmp;

    offs = tb->configWatchdog(&tb->sWD) + 1;	// + 1 weil Ausgang 1 Takt Später erst übernommen wird
	printf("Starte Test %i -> enalways, no zerocmp and cmp = %i \n", testnumber, tb->sWD.compare0);
	while(!tb->readInterrupt()){
		if (tb->readOutput()){
			CRED
			printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
			CDEFAULT
		}
		tb->tick();
	}
	if (!tb->readOutput()){
		CRED
		printf("FEHLER Ausgang nicht gesetzt. Tick:%li \n", tb->m_tickcount-offs);
		CDEFAULT
	}
	if(tb->m_tickcount-offs == (tb->sWD.compare0<<tb->sWD.cfg.field.scale)){
		CGREEN
	}else{
		CRED
	}
    printf("Ende Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, (tb->sWD.compare0<<tb->sWD.cfg.field.scale) );
	CDEFAULT
	tb->checkEndtest();
    testnumber++;

// Test 2    
    tb->sWD.cfg.field.encoreawake = 1;  // Start Counter when corerst = 0
    tb->sWD.cfg.field.scale = std::rand() % 0x4 + 1;
    tb->sWD.cfg.field.zerocmp = 1;
    tb->sWD.cfg.field.enalways = 0; 
	tb->sWD.compare0 = std::rand() % max_cmp;
	printf("Starte Test %i -> encoreawake, zerocmp, scale = %i and cmp = %i \n", testnumber, tb->sWD.cfg.field.scale,tb->sWD.compare0);

    offs = tb->configWatchdog(&tb->sWD) + 1;
	while(!tb->readInterrupt()){
		if (tb->readOutput()){
			CRED
			printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
			CDEFAULT
		}
		tb->tick();
	}
	if (!tb->readOutput()){
		CRED
		printf("FEHLER Ausgang nicht gesetzt. Tick:%li \n", tb->m_tickcount-offs);
		CDEFAULT
	}
	if(tb->m_tickcount-offs == (tb->sWD.compare0<<tb->sWD.cfg.field.scale)){
		CGREEN
	}else{
		CRED
	}
    printf("Ende Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, (tb->sWD.compare0<<tb->sWD.cfg.field.scale) );
    CDEFAULT
	tb->checkEndtest();
	testnumber++;

// Test 3
 	tb->sWD.cfg.field.encoreawake = 1;  // Start Counter when corerst = 0
    tb->sWD.cfg.field.scale = 0;
    tb->sWD.cfg.field.zerocmp = 1;
    tb->sWD.cfg.field.enalways = 0; 
    tb->sWD.cfg.field.en = 0; 
	tb->sWD.compare0 = std::rand() % max_cmp;
	printf("Starte Test %i -> encoreawake, zerocmp, no_output, scale = %i and cmp = %i \n", testnumber, tb->sWD.cfg.field.scale,tb->sWD.compare0);

    offs = tb->configWatchdog(&tb->sWD) + 1;
	while(!tb->readInterrupt()){
		if(state == false && tb->readReg(Watchdog_TB::reg_ctrl_s) == (tb->sWD.compare0/2)){
			printf("Feeding Watchdog Count:%li \n", tb->m_tickcount-offs);
			tb->unlock();
			tb->writeReg(Watchdog_TB::reg_ctrl_feed,0xD09F00D);
			offs = tb->m_tickcount + 1;
			state = true;
		}

		if(tb->readOutput()){
			CRED
			printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
			CDEFAULT
		}
		tb->tick();
	}
	state = false;
	if (tb->readOutput()){
		CRED
		printf("FEHLER Ausgang wurde gesetzt. Tick:%li \n", tb->m_tickcount-offs);
		CDEFAULT
	}
	if(tb->m_tickcount-offs == (tb->sWD.compare0<<tb->sWD.cfg.field.scale)){
		CGREEN
	}else{
		CRED
	}
    printf("Ende Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, (tb->sWD.compare0<<tb->sWD.cfg.field.scale) );
    CDEFAULT
	tb->checkEndtest();
	testnumber++;

#endif
#if defined(MODE_WINDOW) || defined(MODE_BOTH)
// Test 4    
    tb->sWD.cfg.field.deglitch = 1;  // Window Mode
	tb->sWD.cfg.field.interrupt = 0;  
	tb->sWD.cfg.field.scale = 0;
	tb->sWD.cfg.field.encoreawake = 0;  // Start Counter when corerst = 0
	tb->sWD.cfg.field.zerocmp = 0;
	tb->sWD.cfg.field.enalways = 1; // Start Counter
	tb->sWD.cfg.field.en = 1;   // Enable Output
    tb->sWD.compare0 = std::rand() % max_cmp + 2;
    tb->sWD.compare1 = std::rand() % (max_cmp-tb->sWD.compare0) + tb->sWD.compare0;
	printf("Starte Test %i -> encoreawake, zerocmp, scale = 0 and cmp0 = %i and cmp1 = %i \n", testnumber, tb->sWD.compare0, tb->sWD.compare1);

    offs = tb->configWatchdog(&tb->sWD) + 1;
	while(!(tb->readOutput() > 0)){
		if(tb->readOutput()){
			CRED
			printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
			CDEFAULT
		}
		tb->tick();
	}
	if (!tb->readOutput()){
		CRED
		printf("FEHLER Ausgang nicht gesetzt. Tick:%li \n", tb->m_tickcount-offs);
		CDEFAULT
	}
	if(tb->m_tickcount-offs == (tb->sWD.compare1<<tb->sWD.cfg.field.scale)){
		CGREEN
	}else{
		CRED
	}
    printf("Ende Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, (tb->sWD.compare1<<tb->sWD.cfg.field.scale) );
    CDEFAULT
	tb->checkEndtest();
	testnumber++;


// Test 5    
    tb->sWD.cfg.field.encoreawake = 1;  // Start Counter when corerst = 0
    tb->sWD.cfg.field.scale = std::rand() % 0x4 + 1;
    tb->sWD.cfg.field.zerocmp = 1;
    tb->sWD.cfg.field.enalways = 0; 
	tb->sWD.compare0 = std::rand() % max_cmp + 2;
    tb->sWD.compare1 = std::rand() % (max_cmp-tb->sWD.compare0) + tb->sWD.compare0;
	printf("Starte Test %i -> encoreawake, zerocmp, scale = %i and cmp0 = %i cmp1 = %i\n", testnumber, tb->sWD.cfg.field.scale,tb->sWD.compare0, tb->sWD.compare1);

    offs = tb->configWatchdog(&tb->sWD) + 1;
	while(!tb->readInterrupt()){
		if(state == false && tb->readReg(Watchdog_TB::reg_ctrl_s) == ((tb->sWD.compare1-tb->sWD.compare0)/2)+tb->sWD.compare0){
			printf("Feeding Watchdog Count:%li \n", tb->m_tickcount-offs);
			tb->unlock();
			tb->writeReg(Watchdog_TB::reg_ctrl_feed,0xD09F00D);
			offs = tb->m_tickcount + 1;
			state = true;
		}
		if (tb->readOutput()){
			CRED
			printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
			CDEFAULT
		}
		tb->tick();
	}
	state = false;
	if (!tb->readOutput()){
		CRED
		printf("FEHLER Ausgang nicht gesetzt. Tick:%li \n", tb->m_tickcount-offs);
		CDEFAULT
	}
	if(tb->m_tickcount-offs == (tb->sWD.compare1<<tb->sWD.cfg.field.scale)){
		CGREEN
	}else{
		CRED
	}
    printf("Ende Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, (tb->sWD.compare1<<tb->sWD.cfg.field.scale) );
    CDEFAULT
	tb->checkEndtest();
	testnumber++;

// Test 6
 	tb->sWD.cfg.field.encoreawake = 1;  // Start Counter when corerst = 0
    tb->sWD.cfg.field.scale = 0;
    tb->sWD.cfg.field.zerocmp = 1;
    tb->sWD.cfg.field.enalways = 0; 
    tb->sWD.cfg.field.en = 0; 
	printf("Starte Test %i -> encoreawake, zerocmp, no_output, scale = %i and cmp0 = %i cmp1 = %i \n", testnumber, tb->sWD.cfg.field.scale, tb->sWD.compare0, tb->sWD.compare1);

    offs = tb->configWatchdog(&tb->sWD) + 1;
	while(!tb->readInterrupt()){
		if(tb->readOutput()){
			CRED
			printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
			CDEFAULT
		}
		tb->tick();
	}
	if (tb->readOutput()){
		CRED
		printf("FEHLER Ausgang wurde gesetzt. Tick:%li \n", tb->m_tickcount-offs);
		CDEFAULT
	}
	if(tb->m_tickcount-offs == (tb->sWD.compare1<<tb->sWD.cfg.field.scale)){
		CGREEN
	}else{
		CRED
	}
    printf("Ende Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, (tb->sWD.compare1<<tb->sWD.cfg.field.scale) );
    CDEFAULT
	tb->checkEndtest();
	testnumber++;

// Test 7   
    tb->sWD.cfg.field.encoreawake = 1;  // Start Counter when corerst = 0
    tb->sWD.cfg.field.zerocmp = 1;
    tb->sWD.cfg.field.enalways = 0; 
    tb->sWD.cfg.field.en = 1; 
	tb->sWD.compare0 = std::rand() % max_cmp + 2;
    tb->sWD.compare1 = std::rand() % (max_cmp-tb->sWD.compare0) + tb->sWD.compare0;
	printf("Starte Test %i -> encoreawake, zerocmp, scale = %i and cmp0 = %i cmp1 = %i\n", testnumber, tb->sWD.cfg.field.scale,tb->sWD.compare0, tb->sWD.compare1);
    offs = tb->configWatchdog(&tb->sWD) + 1;
	while(!tb->readInterrupt()){
		if (tb->readOutput()){
			CRED
			printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
			CDEFAULT
		}
		if(state == false && tb->readReg(Watchdog_TB::reg_ctrl_s) == (tb->sWD.compare0/2)){
			printf("Feeding Watchdog Count:%li \n", tb->m_tickcount-offs);
			tb->unlock();
			tb->writeReg(Watchdog_TB::reg_ctrl_feed,0xD09F00D);
			state = true;
			offs++;	// tick is following
		}
		tb->tick();
	}
	state = false;
	if (!tb->readOutput()){
		CRED
		printf("FEHLER Ausgang nicht gesetzt. Tick:%li \n", tb->m_tickcount-offs);
		CDEFAULT
	}
	if(tb->m_tickcount-offs == ((tb->sWD.compare0/2)<<tb->sWD.cfg.field.scale)){
		CGREEN
	}else{
		CRED
	}
    printf("Ende Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, ((tb->sWD.compare0/2)<<tb->sWD.cfg.field.scale) );
    CDEFAULT
	tb->checkEndtest();
	testnumber++;


#endif

	for(int i = 0; i<50; i++){
		tb->tick();
	}

	printf("Finished\n");
	tb->closetrace();
	/*while(!tb->done()) {
		
	} exit(EXIT_SUCCESS);*/
}