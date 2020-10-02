#include "watchdogreg.h"
#include "config.h"

#define CRED		printf("\033[0;31m");	// Red Color
#define CGREEN		printf("\033[0;32m");	// Green Color
#define CDEFAULT 	printf("\033[0;0m");	// Default Color

#define CASE_TIMER_REG(id,name) \
	case id: \
	return m_core->io_WDIO_##id##_##name ; \
	break;

#define read_TIMER_REG(name) \
    return m_core->io_regs_##name; \
    break;
#define write_TIMER_REG(name) \
    m_core->io_regs_##name = data; 
#define write_TIMER_REG_valid(name,value) \
    m_core->io_regs_##name = value; 

unsigned int Watchdog_TB::readReg(Register reg){
	switch(reg){
		case reg_ctrl_ip_0: 
            read_TIMER_REG(cfg_read_ip_0)
		case reg_ctrl_running: 
			read_TIMER_REG(cfg_read_running)
		case reg_ctrl_countAlways: 
        	read_TIMER_REG(cfg_read_countAlways)
		case reg_ctrl_deglitch: 
            read_TIMER_REG(cfg_read_deglitch) 
		case reg_ctrl_zerocmp: 
            read_TIMER_REG(cfg_read_zerocmp) 
		case reg_ctrl_sticky: 
            read_TIMER_REG(cfg_read_sticky) 
		case reg_ctrl_scale: 
            read_TIMER_REG(cfg_read_scale) 
		case reg_ctrl_s: 
            read_TIMER_REG(s_read) 
		case reg_ctrl_cmp_0: 
            read_TIMER_REG(cmp_0_read) 
		case reg_ctrl_cmp_1: 
			#if !defined(MODE_TIMEOUT)
				read_TIMER_REG(cmp_1_read) 
			#endif
		case reg_ctrl_countLo: 
            read_TIMER_REG(countLo_read) 
		case reg_ctrl_countHi: 
		    read_TIMER_REG(countHi_read) 
		case reg_ctrl_feed: 
            read_TIMER_REG(feed_read) 
		default: 
			return 0;
			break;
	};
	return 0;
}

void Watchdog_TB::writeReg(Register reg, unsigned int data){
	
	switch(reg){
		case reg_ctrl_ip_0:
            write_TIMER_REG(cfg_write___05Fip_0)
            write_TIMER_REG_valid(cfg_write_ip_0,true)
			break;
		case reg_ctrl_running: 
            write_TIMER_REG(cfg_write___05Frunning)
            write_TIMER_REG_valid(cfg_write_running,true)
			break;
		case reg_ctrl_countAlways: 
            write_TIMER_REG(cfg_write___05FcountAlways)
            write_TIMER_REG_valid(cfg_write_countAlways,true)
			break;
		case reg_ctrl_deglitch: 
            write_TIMER_REG(cfg_write___05Fdeglitch)
            write_TIMER_REG_valid(cfg_write_deglitch,true)
			break;
		case reg_ctrl_zerocmp: 
            write_TIMER_REG(cfg_write___05Fzerocmp)
            write_TIMER_REG_valid(cfg_write_zerocmp,true)
			break;
		case reg_ctrl_sticky: 
            write_TIMER_REG(cfg_write___05Fsticky)
            write_TIMER_REG_valid(cfg_write_sticky,true)
			break;
		case reg_ctrl_scale: 
            write_TIMER_REG(cfg_write___05Fscale)
            write_TIMER_REG_valid(cfg_write_scale,true)
			break;
		case reg_ctrl_s: 
            write_TIMER_REG(s_write_bits)
            write_TIMER_REG_valid(s_write_valid,true)
			break;
		case reg_ctrl_cmp_0: 
            write_TIMER_REG(cmp_0_write_bits)
            write_TIMER_REG_valid(cmp_0_write_valid,true)
			break;
		case reg_ctrl_cmp_1: 
			#if !defined(MODE_TIMEOUT)
				write_TIMER_REG(cmp_1_write_bits)
				write_TIMER_REG_valid(cmp_1_write_valid,true)
			#endif
			break;
		case reg_ctrl_countLo: 
            write_TIMER_REG(countLo_write_bits)
            write_TIMER_REG_valid(countLo_write_valid,true)
			break;
		case reg_ctrl_countHi: 
            write_TIMER_REG(countHi_write_bits)
            write_TIMER_REG_valid(countHi_write_valid,true)
			break;
		case reg_ctrl_feed: 
            write_TIMER_REG(feed_write_bits)
            write_TIMER_REG_valid(feed_write_valid,true)
			break;
		default: break;
	};
	tick();
    
    write_TIMER_REG_valid(countHi_write_valid,false)
    write_TIMER_REG_valid(countLo_write_valid,false)
	#if !defined(MODE_TIMEOUT)
    	write_TIMER_REG_valid(cmp_1_write_valid,false)
	#endif
    write_TIMER_REG_valid(cmp_0_write_valid,false)
    write_TIMER_REG_valid(s_write_valid,false)
    write_TIMER_REG_valid(cfg_write_scale,false)
    write_TIMER_REG_valid(cfg_write_sticky,false)
    write_TIMER_REG_valid(cfg_write_zerocmp,false)
    write_TIMER_REG_valid(cfg_write_deglitch,false)
    write_TIMER_REG_valid(cfg_write_countAlways,false)
    write_TIMER_REG_valid(cfg_write_ip_0,false)
    write_TIMER_REG_valid(feed_write_valid,false)
    lock();
}

unsigned long Watchdog_TB::configWatchdog(struct wd_unit *sWD){
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_scale,sWD->cfg.field.scale);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_zerocmp,sWD->cfg.field.zerocmp);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_deglitch,sWD->cfg.field.deglitch);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_countLo,(sWD->count&0xFFFFFFFF));
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_countHi,((sWD->count&0xFFFFFFFF00000000)>>32));
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_s,sWD->s);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_feed,sWD->feed);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_cmp_0,sWD->compare0);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_cmp_1,sWD->compare1);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_ip_0,sWD->cfg.field.interrupt);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_sticky,sWD->cfg.field.en);
    unlock();
	writeReg(Watchdog_TB::reg_ctrl_countAlways,sWD->cfg.field.enalways);
    unsigned long offs = m_tickcount;
    unlock();
	writeReg(Watchdog_TB::reg_ctrl_running,sWD->cfg.field.encoreawake);
    if(!sWD->cfg.field.enalways){
        offs = m_tickcount;
    }
	return offs;
}

void Watchdog_TB::updateStruct(struct wd_unit *sWD){
	sWD->cfg.field.scale = readReg(Watchdog_TB::reg_ctrl_scale);
	sWD->cfg.field.zerocmp = readReg(Watchdog_TB::reg_ctrl_zerocmp);
	sWD->cfg.field.deglitch = readReg(Watchdog_TB::reg_ctrl_deglitch);
	sWD->cfg.field.enalways = readReg(Watchdog_TB::reg_ctrl_countAlways);
	sWD->cfg.field.encoreawake = readReg(Watchdog_TB::reg_ctrl_running);
	sWD->cfg.field.interrupt = readReg(Watchdog_TB::reg_ctrl_ip_0);
	sWD->count = readReg(Watchdog_TB::reg_ctrl_countLo);
	sWD->count |= (uint64_t)readReg(Watchdog_TB::reg_ctrl_countHi)<<32;
	sWD->s = readReg(Watchdog_TB::reg_ctrl_s);
	sWD->feed = readReg(Watchdog_TB::reg_ctrl_feed);
	sWD->compare0 = readReg(Watchdog_TB::reg_ctrl_cmp_0);
	sWD->cfg.field.en = readReg(Watchdog_TB::reg_ctrl_sticky);

}

void Watchdog_TB::unlock(void){
	m_core->io_unlocked = true;
}

void Watchdog_TB::lock(void){
	m_core->io_unlocked = false;
}

void Watchdog_TB::disableWatchdog(void){
    unlock();
    writeReg(Watchdog_TB::reg_ctrl_sticky,0);
    unlock();
	writeReg(Watchdog_TB::reg_ctrl_countAlways,0);
    unlock();
	writeReg(Watchdog_TB::reg_ctrl_running,0);
}

unsigned int Watchdog_TB::readInterrupt(){
	return m_core->io_ip_0;
}

unsigned int Watchdog_TB::readOutput(){
	return m_core->io_rst;
}

void Watchdog_TB::resetOutput(){
    m_core->io_output_reset = true;
    tick();
    m_core->io_output_reset = false;
}

void Watchdog_TB::resetInterrupt(){
	unlock();
    writeReg(Watchdog_TB::reg_ctrl_ip_0,0);
}

void Watchdog_TB::resetCounter(){
	unlock();
    writeReg(Watchdog_TB::reg_ctrl_countLo,0);
	unlock();
    writeReg(Watchdog_TB::reg_ctrl_countHi,0);
}

void Watchdog_TB::init(){
	reset();
	resetOutput();    
}

void Watchdog_TB::checkEndtest(){
	disableWatchdog();
	if(readReg(Watchdog_TB::reg_ctrl_s) >= readReg(Watchdog_TB::reg_ctrl_cmp_0) && readReg(Watchdog_TB::reg_ctrl_zerocmp) && !readReg(Watchdog_TB::reg_ctrl_deglitch)){
		CRED
		printf("FEHLER Zähler wurde nicht zurückgesetzt trotz zerocmp Bit.\n");
		CDEFAULT
	}
	if(readReg(Watchdog_TB::reg_ctrl_s)  >= readReg(Watchdog_TB::reg_ctrl_cmp_1) && readReg(Watchdog_TB::reg_ctrl_zerocmp) && readReg(Watchdog_TB::reg_ctrl_deglitch)){
		CRED
		printf("FEHLER Zähler wurde nicht zurückgesetzt trotz zerocmp Bit.\n");
		CDEFAULT
	}
	
	uint64_t ocounter = readReg(Watchdog_TB::reg_ctrl_countLo);
	ocounter |= ((uint64_t)readReg(Watchdog_TB::reg_ctrl_countHi)<<32);
	for(uint8_t i = 0; i < 10; i++){
		tick();
	}
	uint64_t ncounter = readReg(Watchdog_TB::reg_ctrl_countLo);
	ncounter |= ((uint64_t)readReg(Watchdog_TB::reg_ctrl_countHi)<<32);
	if(ncounter != ocounter){
		CRED
		printf("FEHLER Zähler wurde nicht gestoppt.\n");
		CDEFAULT
	}

	resetCounter();
	// Check Output
	if(!readOutput() && readReg(Watchdog_TB::reg_ctrl_sticky)){
		CRED
		printf("FEHLER Ausgang durch Zähler zurückgesetzt.\n");
		CDEFAULT
	}
    resetOutput();
	if(readOutput()){
		CRED
		printf("FEHLER Ausgang nicht zurückgesetzt.\n");
		CDEFAULT
	}
	// Check Interrupt
	if (!readInterrupt()){
		CRED
		printf("FEHLER Interrupt wurde durch Zähler zurückgesetzt.\n");
		CDEFAULT
	}
    resetInterrupt();
	if (readInterrupt()){
		CRED
		printf("FEHLER Interrupt nicht zurückgesetzt.\n");
		CDEFAULT
	}
}

