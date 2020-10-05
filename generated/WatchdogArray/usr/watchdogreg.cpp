#include "watchdogreg.h"

uint32_t Watchdog_TB::readReg(Register reg, uint8_t timer){
	switch(reg){
		case reg_ctrl_ip_0: 
			read_CASE_TIMER_REG(timerIO_regs_cfg_read_ip_0)
		case reg_ctrl_running: 
			read_CASE_TIMER_REG(timerIO_regs_cfg_read_running)
		case reg_ctrl_countAlways:
			read_CASE_TIMER_REG(timerIO_regs_cfg_read_countAlways)
		case reg_ctrl_deglitch:
			read_CASE_TIMER_REG(timerIO_regs_cfg_read_deglitch) 
		case reg_ctrl_zerocmp: 
			read_CASE_TIMER_REG(timerIO_regs_cfg_read_zerocmp)
		case reg_ctrl_sticky: 
			read_CASE_TIMER_REG(timerIO_regs_cfg_read_sticky)
		case reg_ctrl_scale: 
			read_CASE_TIMER_REG(timerIO_regs_cfg_read_scale)
		case reg_ctrl_s: 
			read_CASE_TIMER_REG(timerIO_regs_s_read)
		case reg_ctrl_cmp_0: 
			read_CASE_TIMER_REG(timerIO_regs_cmp_0_read)
		case reg_ctrl_cmp_1: 
			#if defined(MODE_TIMEOUT)
				return 0;
			#else
				read_CASE_TIMER_REG(timerIO_regs_cmp_1_read)
			#endif
		case reg_ctrl_countLo:
			read_CASE_TIMER_REG(timerIO_regs_countLo_read) 
		case reg_ctrl_countHi: 
			read_CASE_TIMER_REG(timerIO_regs_countHi_read)
		case reg_ctrl_feed: 
			read_CASE_TIMER_REG(timerIO_regs_feed_read)
		case reg_ctrl_mux: 
			read_CASE_TIMER_REG(mux_read)
		case reg_ctrl_pulsewidth: 
			read_CASE_TIMER_REG(pulsewidth_read)
		default: 
			return 0;
			break;
	};
	return 0;
};

void Watchdog_TB::writeReg(Register reg,uint32_t data, uint8_t timer ){
	switch(reg){
		case reg_ctrl_ip_0:
			write_CASE_TIMER_REG(timerIO_regs_cfg_write___05Fip_0, timerIO_regs_cfg_write_ip_0)
			break;
		case reg_ctrl_running: 
			write_CASE_TIMER_REG(timerIO_regs_cfg_write___05Frunning,timerIO_regs_cfg_write_running)
			break;
		case reg_ctrl_countAlways: 
			write_CASE_TIMER_REG(timerIO_regs_cfg_write___05FcountAlways,timerIO_regs_cfg_write_countAlways)
			break;
		case reg_ctrl_deglitch: 	
			write_CASE_TIMER_REG(timerIO_regs_cfg_write___05Fdeglitch,timerIO_regs_cfg_write_deglitch)
			break;
		case reg_ctrl_zerocmp: 
			write_CASE_TIMER_REG(timerIO_regs_cfg_write___05Fzerocmp,timerIO_regs_cfg_write_zerocmp)
			break;
		case reg_ctrl_sticky:
			write_CASE_TIMER_REG(timerIO_regs_cfg_write___05Fsticky,timerIO_regs_cfg_write_sticky)
			break;
		case reg_ctrl_scale: 
			write_CASE_TIMER_REG(timerIO_regs_cfg_write___05Fscale,timerIO_regs_cfg_write_scale)
			break;
		case reg_ctrl_s: 
			write_CASE_TIMER_REG(timerIO_regs_s_write_bits,timerIO_regs_s_write_valid)
			break;
		case reg_ctrl_cmp_0: 
			write_CASE_TIMER_REG(timerIO_regs_cmp_0_write_bits,timerIO_regs_cmp_0_write_valid)
			break;
		case reg_ctrl_cmp_1: 
			#if defined(MODE_BOTH) || defined(MODE_WINDOW)
				write_CASE_TIMER_REG(timerIO_regs_cmp_1_write_bits,timerIO_regs_cmp_1_write_valid)
			#endif
			break;
		case reg_ctrl_countLo: 
			write_CASE_TIMER_REG(timerIO_regs_countLo_write_bits,timerIO_regs_countLo_write_valid)
			break;
		case reg_ctrl_countHi: 
			write_CASE_TIMER_REG(timerIO_regs_countHi_write_bits,timerIO_regs_countHi_write_valid)
			break;
		case reg_ctrl_feed: 
			write_CASE_TIMER_REG(timerIO_regs_feed_write_bits,timerIO_regs_feed_write_valid)
			break;
		case reg_ctrl_mux: 
			write_CASE_TIMER_REG(mux_write_bits,mux_write_valid)
			break;
		case reg_ctrl_pulsewidth: 
			write_CASE_TIMER_REG(pulsewidth_write_bits,pulsewidth_write_valid)
			break;
		default: break;
	};
	tick();

    write_TIMER_REGS_valid(pulsewidth_write_valid,false)
    write_TIMER_REGS_valid(mux_write_valid,false)
	write_TIMER_REGS_valid(timerIO_regs_feed_write_valid,false)    
    write_TIMER_REGS_valid(timerIO_regs_countHi_write_valid,false)
    write_TIMER_REGS_valid(timerIO_regs_countLo_write_valid,false)
	#if defined(MODE_BOTH) || defined(MODE_WINDOW)
    	write_TIMER_REGS_valid(timerIO_regs_cmp_1_write_valid,false)
	#endif
    write_TIMER_REGS_valid(timerIO_regs_cmp_0_write_valid,false)
    write_TIMER_REGS_valid(timerIO_regs_s_write_valid,false)
    write_TIMER_REGS_valid(timerIO_regs_cfg_write_scale,false)
    write_TIMER_REGS_valid(timerIO_regs_cfg_write_sticky,false)
    write_TIMER_REGS_valid(timerIO_regs_cfg_write_zerocmp,false)
    write_TIMER_REGS_valid(timerIO_regs_cfg_write_deglitch,false)
    write_TIMER_REGS_valid(timerIO_regs_cfg_write_countAlways,false)
    write_TIMER_REGS_valid(timerIO_regs_cfg_write_running,false)
    write_TIMER_REGS_valid(timerIO_regs_cfg_write_ip_0,false)

    lock();
};

unsigned long  Watchdog_TB::configWatchdog(struct wd_unit *sWD, uint8_t timer){
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_scale,sWD->cfg.field.scale,timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_zerocmp,sWD->cfg.field.zerocmp,timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_deglitch,sWD->cfg.field.deglitch,timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_countLo,(sWD->count&0xFFFFFFFF),timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_countHi,((sWD->count&0xFFFFFFFF00000000)>>32),timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_s,sWD->s,timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_feed,sWD->feed,timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_cmp_0,sWD->compare0,timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_cmp_1,sWD->compare1,timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_pulsewidth,sWD->pulsewidth,timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_mux,sWD->mux,timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_sticky,sWD->cfg.field.en,timer);
    unlock();
	writeReg(Watchdog_TB::reg_ctrl_ip_0,sWD->cfg.field.interrupt,timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_countAlways,sWD->cfg.field.enalways,timer);
    unsigned long offs = m_tickcount-1;
    unlock();
	writeReg(Watchdog_TB::reg_ctrl_running,sWD->cfg.field.encoreawake,timer);
    if(!sWD->cfg.field.enalways){
        offs = m_tickcount-1;
    }
	return offs;
}

void Watchdog_TB::updateStruct(struct wd_unit *sWD, uint8_t timer){
	sWD->cfg.field.scale = readReg(Watchdog_TB::reg_ctrl_scale,timer);
	sWD->cfg.field.zerocmp = readReg(Watchdog_TB::reg_ctrl_zerocmp,timer);
	sWD->cfg.field.deglitch = readReg(Watchdog_TB::reg_ctrl_deglitch,timer);
	sWD->cfg.field.enalways = readReg(Watchdog_TB::reg_ctrl_countAlways,timer);
	sWD->cfg.field.encoreawake = readReg(Watchdog_TB::reg_ctrl_running,timer);
	sWD->cfg.field.interrupt = readReg(Watchdog_TB::reg_ctrl_ip_0,timer);
	sWD->count = readReg(Watchdog_TB::reg_ctrl_countLo,timer);
	sWD->count |= (uint64_t)readReg(Watchdog_TB::reg_ctrl_countHi,timer)<<32;
	sWD->s = readReg(Watchdog_TB::reg_ctrl_s,timer);
	sWD->feed = readReg(Watchdog_TB::reg_ctrl_feed,timer);
	sWD->compare0 = readReg(Watchdog_TB::reg_ctrl_cmp_0,timer);
	sWD->compare1 = readReg(Watchdog_TB::reg_ctrl_cmp_1,timer);
	sWD->cfg.field.en = readReg(Watchdog_TB::reg_ctrl_sticky,timer);
	sWD->pulsewidth = readReg(Watchdog_TB::reg_ctrl_pulsewidth,timer);
	sWD->mux = readReg(Watchdog_TB::reg_ctrl_mux,timer);
}

void Watchdog_TB::unlock(void){
	m_core->io_key_write_valid = true;
	m_core->io_key_write_bits = Key;
	tick();
	m_core->io_key_write_valid = false;
	if(WD_PRBS){
		calc_key();
	}
}
void Watchdog_TB::lock(void){
	m_core->io_key_write_valid = true;
	m_core->io_key_write_bits = 0;
	tick();
	m_core->io_key_write_valid = false;
}

void Watchdog_TB::calc_key(void){
	uint64_t init = Key;
    uint64_t poly = PRBS_polynom;
    uint8_t bit = 0;
    for(uint8_t i = 0; i<32; i++){
        if ((poly & (1<<0)) > 0){
            bit ^= init;
        }
        init >>= 1;
        poly >>= 1;
    }
    bit &= 0x01;
    Key = ((Key << 1) | bit) & Key_Mask;
}

void Watchdog_TB::disableWatchdog(uint8_t timer){
    unlock();
    writeReg(Watchdog_TB::reg_ctrl_sticky,0,timer);
    unlock();
	writeReg(Watchdog_TB::reg_ctrl_countAlways,0,timer);
    unlock();
	writeReg(Watchdog_TB::reg_ctrl_running,0,timer);
}

void Watchdog_TB::writeInv(uint32_t data){
	m_core->io_inv_write_valid = true;
	m_core->io_inv_write_bits = data;
	tick();
	m_core->io_inv_write_valid = false;
}

void Watchdog_TB::resetInterrupt(uint8_t interrupt){
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_ip_0,0,interrupt);
}

void Watchdog_TB::resetCounter(uint8_t timer){
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_countLo,0,timer);
	unlock();
	writeReg(Watchdog_TB::reg_ctrl_countHi,0,timer);
}

uint8_t Watchdog_TB::readInterrupt(uint8_t interrupt){
	if(interrupt < WD_Ints){	// First use Signals
		read_CASE_Interrupt
	}else{	// If no Signal then use Register
		return (uint8_t)readReg(Watchdog_TB::reg_ctrl_ip_0,interrupt);
	}
	
	return 0;
}

uint8_t Watchdog_TB::readOutput(uint8_t output){
	read_CASE_Output
	return 0;
}

