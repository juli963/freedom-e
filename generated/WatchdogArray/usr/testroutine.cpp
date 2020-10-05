#include "testroutine.h"


#define CRED		printf("\033[0;31m"); glerror = true;	// Red Color Output
#define CGREEN		printf("\033[0;32m");	// Green Color
#define CDEFAULT 	printf("\033[0;0m");	// Default Color

void TestRoutines::TestSingleWD(Watchdog_TB* tb,uint8_t Dog,uint8_t Output){
	unsigned long offs = 0;
	bool state, error = false;

    printf("\n");
    printf("----- Dog %i Ausgang %i Test -----\n", Dog,Output);
    tb->sWD.mux |= ((uint32_t)1<<Output);

    tb->unlock();
    tb->writeReg(Watchdog_TB::reg_ctrl_deglitch,0,Dog);
    #if defined(MODE_WINDOW) 
            if(!tb->readReg(Watchdog_TB::reg_ctrl_deglitch)){
                CRED
                printf("FEHLER Mode Register hat den falschen Wert\n");
                CDEFAULT
            }
    #elif defined(MODE_TIMEOUT) || defined(MODE_BOTH)
            if(tb->readReg(Watchdog_TB::reg_ctrl_deglitch,Dog)){
                CRED
                printf("FEHLER Mode Register hat den falschen Wert\n");
                CDEFAULT
            }
    #endif
        tb->unlock();
        tb->writeReg(Watchdog_TB::reg_ctrl_deglitch,1,Dog);
    #if defined(MODE_TIMEOUT) 
            if(tb->readReg(Watchdog_TB::reg_ctrl_deglitch)){
                CRED
                printf("FEHLER Mode Register hat den falschen Wert\n");
                CDEFAULT
            }
    #elif defined(MODE_WINDOW) || defined(MODE_BOTH)
            if(!tb->readReg(Watchdog_TB::reg_ctrl_deglitch,Dog)){
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
        tb->sWD.compare0 = std::rand() % (max_cmp-1)+1;
        tb->sWD.pulsewidth = std::rand() % (max_cmp-1)+1;
        

        offs = tb->configWatchdog(&tb->sWD,Dog) + 1;	// + 1 weil Ausgang 1 Takt Später erst übernommen wird
        printf("Starte Test %i -> enalways, no zerocmp and cmp = %i \n", testnumber, tb->sWD.compare0);
        while(!tb->readInterrupt(Dog)){
            if (tb->readOutput(Output) && !error){
                CRED
                error = true;
                printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
                CDEFAULT
            }
            tb->tick();
        }
        error = false;
        if (!tb->readOutput(Output)){
            CRED
            printf("FEHLER Ausgang nicht gesetzt. Tick:%li \n", tb->m_tickcount-offs);
            CDEFAULT
        }
        if(tb->m_tickcount-offs == (tb->sWD.compare0<<tb->sWD.cfg.field.scale)){
            CGREEN
        }else{
            CRED
        }
        printf("Ausgang Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, ((tb->sWD.compare0)<<tb->sWD.cfg.field.scale) );
        CDEFAULT
        offs = tb->m_tickcount+1;
        while(tb->readOutput(Output)){
            tb->tick();
        }
        if(tb->m_tickcount-offs == tb->sWD.pulsewidth){
            CGREEN
        }else{
            CRED
        }
        printf("Ende Test %i Pulsbreitenzyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, tb->sWD.pulsewidth );
        CDEFAULT
        checkEndtest(tb,Dog,Output);
        testnumber++;

    // Test 2    
        tb->sWD.cfg.field.encoreawake = 1;  // Start Counter when corerst = 0
        tb->sWD.cfg.field.scale = std::rand() % 0x4 + 1;
        tb->sWD.cfg.field.zerocmp = 1;
        tb->sWD.cfg.field.enalways = 0; 
        tb->sWD.compare0 = std::rand() % (max_cmp-1)+1;
        tb->sWD.pulsewidth = std::rand() % (max_cmp-1)+1;
        printf("Starte Test %i -> encoreawake, zerocmp, scale = %i and cmp = %i \n", testnumber, tb->sWD.cfg.field.scale,tb->sWD.compare0);

        offs = tb->configWatchdog(&tb->sWD,Dog) + 1;
        while(!tb->readInterrupt(Dog)){
            if (tb->readOutput(Output) && !error){
                error = true;
                CRED
                printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
                CDEFAULT
            }
            tb->tick();
        }
        error = false;
        if (!tb->readOutput(Output)){
            CRED
            printf("FEHLER Ausgang nicht gesetzt. Tick:%li \n", tb->m_tickcount-offs);
            CDEFAULT
        }
        if(tb->m_tickcount-offs == (tb->sWD.compare0<<tb->sWD.cfg.field.scale)){
            CGREEN
        }else{
            CRED
        }
        printf("Ausgang Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, ((tb->sWD.compare0)<<tb->sWD.cfg.field.scale) );
        CDEFAULT
        offs = tb->m_tickcount+1;
        while(tb->readOutput(Output)){
            tb->tick();
        }
        if(tb->m_tickcount-offs == tb->sWD.pulsewidth){
            CGREEN
        }else{
            CRED
        }
        printf("Ende Test %i Pulsbreitenzyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, tb->sWD.pulsewidth );
        CDEFAULT
        checkEndtest(tb,Dog,Output);
        testnumber++;

    // Test 3
        tb->sWD.cfg.field.encoreawake = 1;  // Start Counter when corerst = 0
        tb->sWD.cfg.field.scale = 0;
        tb->sWD.cfg.field.zerocmp = 0;
        tb->sWD.cfg.field.enalways = 0; 
        tb->sWD.cfg.field.en = 0; 
        tb->sWD.compare0 = std::rand() % max_cmp;
        tb->sWD.pulsewidth = std::rand() % (max_cmp-1)+1;
        printf("Starte Test %i -> encoreawake, no_output, scale = %i and cmp = %i \n", testnumber, tb->sWD.cfg.field.scale,tb->sWD.compare0);

        offs = tb->configWatchdog(&tb->sWD,Dog) + 1;
        while(!tb->readInterrupt(Dog)){
            if(state == false && tb->readReg(Watchdog_TB::reg_ctrl_s,Dog) == (tb->sWD.compare0/2)){
                printf("Feeding Watchdog Count:%li \n", tb->m_tickcount-offs);
                tb->unlock();
                tb->writeReg(Watchdog_TB::reg_ctrl_feed,0xD09F00D,Dog);
                offs = tb->m_tickcount;
                state = true;
            }

            if(tb->readOutput(Output) && !error){
                error = true;
                CRED
                printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
                CDEFAULT
            }
            tb->tick();
        }
        error = false;
        state = false;
        if (tb->readOutput(Output)){
            CRED
            printf("FEHLER Ausgang wurde gesetzt. Tick:%li \n", tb->m_tickcount-offs);
            CDEFAULT
        }
        if(tb->m_tickcount-offs == (tb->sWD.compare0<<tb->sWD.cfg.field.scale)){
            CGREEN
        }else{
            CRED
        }
        printf("Ausgang Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, ((tb->sWD.compare0)<<tb->sWD.cfg.field.scale) );
        CDEFAULT
        tb->unlock();
        tb->writeReg(Watchdog_TB::reg_ctrl_sticky,1,Dog); // Enable Output
        offs = tb->m_tickcount+1;
        while(tb->readOutput(Output)){
            tb->tick();
        }
        if(tb->m_tickcount-offs == tb->sWD.pulsewidth){
            CGREEN
        }else{
            CRED
        }
        printf("Ende Test %i Pulsbreitenzyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, tb->sWD.pulsewidth );
        CDEFAULT
        checkEndtest(tb,Dog,Output);
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
        tb->sWD.pulsewidth = std::rand() % (max_cmp-1)+1;
        printf("Starte Test %i -> encoreawake, zerocmp, scale = 0 and cmp0 = %i and cmp1 = %i \n", testnumber, tb->sWD.compare0, tb->sWD.compare1);

        offs = tb->configWatchdog(&tb->sWD,Dog) + 1;
        while(!(tb->readOutput(Output) > 0)){
            if(tb->readOutput(Output) && !error){
                error = true;
                CRED
                printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
                CDEFAULT
            }
            tb->tick();
        }
        error = false;
        if (!tb->readOutput(Output)){
            CRED
            printf("FEHLER Ausgang nicht gesetzt. Tick:%li \n", tb->m_tickcount-offs);
            CDEFAULT
        }
        if(tb->m_tickcount-offs == (tb->sWD.compare1<<tb->sWD.cfg.field.scale)){
            CGREEN
        }else{
            CRED
        }
        printf("Ausgang Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, ((tb->sWD.compare1)<<tb->sWD.cfg.field.scale) );
        CDEFAULT
        offs = tb->m_tickcount+1;
        while(tb->readOutput(Output)){
            tb->tick();
        }
        if(tb->m_tickcount-offs == tb->sWD.pulsewidth){
            CGREEN
        }else{
            CRED
        }
        printf("Ende Test %i Pulsbreitenzyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, tb->sWD.pulsewidth );
        CDEFAULT
        checkEndtest(tb,Dog,Output);
        testnumber++;


    // Test 5    
        tb->sWD.cfg.field.encoreawake = 1;  // Start Counter when corerst = 0
        tb->sWD.cfg.field.scale = std::rand() % 0x4 + 1;
        tb->sWD.cfg.field.zerocmp = 1;
        tb->sWD.cfg.field.enalways = 0; 
        tb->sWD.compare0 = std::rand() % (max_cmp-2) + 2;
        tb->sWD.compare1 = std::rand() % (max_cmp-tb->sWD.compare0) + tb->sWD.compare0;
        tb->sWD.pulsewidth = std::rand() % (max_cmp-1)+1;
        printf("Starte Test %i -> encoreawake, zerocmp, scale = %i and cmp0 = %i cmp1 = %i\n", testnumber, tb->sWD.cfg.field.scale,tb->sWD.compare0, tb->sWD.compare1);

        offs = tb->configWatchdog(&tb->sWD,Dog) + 1;
        while(!tb->readInterrupt(Dog)){
            if(state == false && tb->readReg(Watchdog_TB::reg_ctrl_s,Dog) == ((tb->sWD.compare1-tb->sWD.compare0)/2)+tb->sWD.compare0){
                printf("Feeding Watchdog Count:%li \n", tb->m_tickcount-offs);
                tb->unlock();
                tb->writeReg(Watchdog_TB::reg_ctrl_feed,0xD09F00D,Dog);
                offs = tb->m_tickcount;
                state = true;
            }
            if (tb->readOutput(Output) && !error){
                error = true;
                CRED
                printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
                CDEFAULT
            }
            tb->tick();
        }
        error = false;
        state = false;
        if (!tb->readOutput(Output)){
            CRED
            printf("FEHLER Ausgang nicht gesetzt. Tick:%li \n", tb->m_tickcount-offs);
            CDEFAULT
        }
        if(tb->m_tickcount-offs == (tb->sWD.compare1<<tb->sWD.cfg.field.scale)){
            CGREEN
        }else{
            CRED
        }
        printf("Ausgang Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, (tb->sWD.compare1<<tb->sWD.cfg.field.scale) );
        CDEFAULT
        offs = tb->m_tickcount+1;
        while(tb->readOutput(Output)){
            tb->tick();
        }
        if(tb->m_tickcount-offs == tb->sWD.pulsewidth){
            CGREEN
        }else{
            CRED
        }
        printf("Ende Test %i Pulsbreitenzyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, tb->sWD.pulsewidth );
        CDEFAULT
        checkEndtest(tb,Dog,Output);
        testnumber++;

    // Test 6
        tb->sWD.cfg.field.encoreawake = 1;  // Start Counter when corerst = 0
        tb->sWD.cfg.field.scale = 0;
        tb->sWD.cfg.field.zerocmp = 0;
        tb->sWD.cfg.field.enalways = 0; 
        tb->sWD.cfg.field.en = 0; 
        tb->sWD.pulsewidth = std::rand() % (max_cmp-1)+1;
        printf("Starte Test %i -> encoreawake, no_output, scale = %i and cmp0 = %i cmp1 = %i \n", testnumber, tb->sWD.cfg.field.scale, tb->sWD.compare0, tb->sWD.compare1);

        offs = tb->configWatchdog(&tb->sWD,Dog) + 1;
        while(!tb->readInterrupt(Dog)){
            if(tb->readOutput(Output) && !error){
                error = true;
                CRED
                printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
                CDEFAULT
            }
            tb->tick();
        }
        error = false;
        if (tb->readOutput(Output)){
            CRED
            printf("FEHLER Ausgang wurde gesetzt. Tick:%li \n", tb->m_tickcount-offs);
            CDEFAULT
        }
        if(tb->m_tickcount-offs == (tb->sWD.compare1<<tb->sWD.cfg.field.scale)){
            CGREEN
        }else{
            CRED
        }
        printf("Ausgang Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, ((tb->sWD.compare0/2)<<tb->sWD.cfg.field.scale) );
        CDEFAULT
        tb->unlock();
        tb->writeReg(Watchdog_TB::reg_ctrl_sticky,1,Dog); // Enable Output
        offs = tb->m_tickcount+1;
        while(tb->readOutput(Output)){
            tb->tick();
        }
        if(tb->m_tickcount-offs == tb->sWD.pulsewidth){
            CGREEN
        }else{
            CRED
        }
        printf("Ende Test %i Pulsbreitenzyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, tb->sWD.pulsewidth );
        CDEFAULT
        checkEndtest(tb,Dog,Output);
        testnumber++;

    // Test 7   
        tb->sWD.cfg.field.encoreawake = 1;  // Start Counter when corerst = 0
        tb->sWD.cfg.field.zerocmp = 1;
        tb->sWD.cfg.field.enalways = 0; 
        tb->sWD.cfg.field.en = 1; 
        tb->sWD.compare0 = std::rand() % (max_cmp-2) + 2;
        tb->sWD.compare1 = std::rand() % (max_cmp-tb->sWD.compare0) + tb->sWD.compare0;
        tb->sWD.pulsewidth = std::rand() % (max_cmp-1)+1;
        printf("Starte Test %i -> encoreawake, zerocmp, scale = %i and cmp0 = %i cmp1 = %i\n", testnumber, tb->sWD.cfg.field.scale,tb->sWD.compare0, tb->sWD.compare1);
        offs = tb->configWatchdog(&tb->sWD,Dog) + 1;
        while(!tb->readInterrupt(Dog)){
            if (tb->readOutput(Output) && !error){
                error = true;
                CRED
                printf("FEHLER Ausgang zu früh gesetzt. Tick:%li \n", tb->m_tickcount-offs);
                CDEFAULT
            }
            if(state == false && tb->readReg(Watchdog_TB::reg_ctrl_s,Dog) == (tb->sWD.compare0/2)){
                printf("Feeding Watchdog Count:%li \n", tb->m_tickcount-offs);
                tb->unlock();
                tb->writeReg(Watchdog_TB::reg_ctrl_feed,0xD09F00D,Dog);
                state = true;
                offs = offs + 3;	// 3 ticks 
            }
            tb->tick();
        }
        error = false;
        state = false;
        if (!tb->readOutput(Output)){
            CRED
            printf("FEHLER Ausgang nicht gesetzt. Tick:%li \n", tb->m_tickcount-offs);
            CDEFAULT
        }
        if(tb->m_tickcount-offs == ((tb->sWD.compare0/2)<<tb->sWD.cfg.field.scale)){
            CGREEN
        }else{
            CRED
        }
        printf("Ausgang Test %i Zyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, ((tb->sWD.compare0/2)<<tb->sWD.cfg.field.scale) );
        CDEFAULT
        offs = tb->m_tickcount-1;
        while(tb->readOutput(Output)){
            tb->tick();
        }
        if(tb->m_tickcount-offs == tb->sWD.pulsewidth){
            CGREEN
        }else{
            CRED
        }
        printf("Ende Test %i Pulsbreitenzyklen Ist = %li Soll = %i \n", testnumber,tb->m_tickcount-offs, tb->sWD.pulsewidth );
        CDEFAULT
        checkEndtest(tb,Dog,Output);
        testnumber++;

    #endif

    tb->unlock();
    tb->writeReg(Watchdog_TB::reg_ctrl_mux,0,Dog);
    CDEFAULT
}

void TestRoutines::TestInversion(Watchdog_TB* tb){
    bool error = false;
	for(uint32_t i = 0; i < WD_Resets; i++){
        tb->unlock();
        tb->writeInv(0);
        tb->tick();
        for(uint8_t x = 0; x < WD_Resets; x++){
            if(tb->readOutput(x) != 0){
                error = true;
                CRED
                printf("FEHLER: Ausgang %i ist nicht 0.\n", x);
                CDEFAULT
            }
        }
        tb->unlock();
		tb->writeInv((1<<i));
        tb->tick();
        for(uint8_t x = 0; x < WD_Resets; x++){
            if(tb->readOutput(x) != 0 && x != i){
                error = true;
                CRED
                printf("FEHLER: Ausgang %i ist nicht 0 obwohl Ausgang %i invertiert werden soll.\n", x, i);
                CDEFAULT
            }else if(tb->readOutput(x) != 1 && x == i){
                error = true;
                CRED
                printf("FEHLER: Ausgang %i ist nicht 1.\n", x);
                CDEFAULT
            }
        }
	}
    tb->unlock();
    tb->writeInv(0);
    if(!error){
        CGREEN
        printf("ERFOLGREICH: Inversionstest\n");
        CDEFAULT
    }
    CDEFAULT
}

void TestRoutines::checkEndtest(Watchdog_TB* tb,uint8_t Dog, uint8_t Output){
	tb->disableWatchdog(Dog);
	if(tb->readReg(Watchdog_TB::reg_ctrl_s,Dog) >= tb->readReg(Watchdog_TB::reg_ctrl_cmp_0,Dog) && tb->readReg(Watchdog_TB::reg_ctrl_zerocmp,Dog) && !tb->readReg(Watchdog_TB::reg_ctrl_deglitch,Dog)){
		CRED
		printf("FEHLER Zähler wurde nicht zurückgesetzt trotz zerocmp Bit.\n");
		CDEFAULT
	}
	if(tb->readReg(Watchdog_TB::reg_ctrl_s,Dog)  >= tb->readReg(Watchdog_TB::reg_ctrl_cmp_1,Dog) && tb->readReg(Watchdog_TB::reg_ctrl_zerocmp,Dog) && tb->readReg(Watchdog_TB::reg_ctrl_deglitch,Dog)){
		CRED
		printf("FEHLER Zähler wurde nicht zurückgesetzt trotz zerocmp Bit.\n");
		CDEFAULT
	}
	
	uint64_t ocounter = tb->readReg(Watchdog_TB::reg_ctrl_countLo,Dog);
	ocounter |= ((uint64_t)tb->readReg(Watchdog_TB::reg_ctrl_countHi,Dog)<<32);
	for(uint8_t i = 0; i < 10; i++){
		tb->tick();
	}
	uint64_t ncounter = tb->readReg(Watchdog_TB::reg_ctrl_countLo,Dog);
	ncounter |= ((uint64_t)tb->readReg(Watchdog_TB::reg_ctrl_countHi,Dog)<<32);
	if(ncounter != ocounter){
		CRED
		printf("FEHLER Zähler wurde nicht gestoppt.\n");
		CDEFAULT
	}

	tb->resetCounter(Dog);
	// Check Interrupt
	if (!tb->readInterrupt(Dog)){
		CRED
		printf("FEHLER Interrupt wurde durch Zähler zurückgesetzt.\n");
		CDEFAULT
	}
    tb->resetInterrupt(Dog);
	if (tb->readInterrupt(Dog)){
		CRED
		printf("FEHLER Interrupt nicht zurückgesetzt.\n");
		CDEFAULT
	}
}
