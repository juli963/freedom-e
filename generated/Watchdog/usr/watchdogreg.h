#include "testbench.h"
class	Watchdog_TB : public TESTBENCH<VWatchdogTimer> {
	public:

	typedef union
	{
		uint64_t cfg; // integer Representation
		struct  
		{
			unsigned int scale : 4;   
			unsigned int rsv0 : 4;          
			unsigned int en : 1;          
			unsigned int zerocmp : 1;          
			unsigned int deglitch : 1;             
			unsigned int rsv1 : 1;          
			unsigned int enalways : 1;          
			unsigned int encoreawake : 1;       // not used in TileLink   
			unsigned int rsv2 : 14;          
			unsigned int interrupt : 1;          
			unsigned int rsv3 : 3;          
			unsigned int rsv4 : 32;          
		} field;
	} wd_config;

	struct wd_unit {
		wd_config cfg;
		uint64_t count;
		uint64_t s; // ??? What was that
		uint32_t feed;
        uint32_t key; // -> Reserved
		uint32_t compare0;
		uint32_t compare1;
	};


	enum Register { 
		reg_ctrl_ip_0,
		reg_ctrl_running, // coreawake
		reg_ctrl_countAlways, //enalways
		reg_ctrl_deglitch,
		reg_ctrl_zerocmp,	// zerocmp
		reg_ctrl_sticky,	// En
		reg_ctrl_scale,
		reg_ctrl_s,
		reg_ctrl_cmp_0,
		reg_ctrl_cmp_1,
		reg_ctrl_countLo,
		reg_ctrl_countHi,
		reg_ctrl_feed
	};
	struct wd_unit sWD;

	unsigned int readReg(Register reg);
	void writeReg(Register reg,unsigned int data);
	unsigned int readInterrupt();
	unsigned int readOutput();
	void resetOutput();
	void resetCounter();
	void resetInterrupt();
	void disableWatchdog();
	unsigned long configWatchdog(struct wd_unit *sWD);	// Return tickcount
	void updateStruct(struct wd_unit *sWD);
    void unlock(void);
	void lock(void);
	void init(void);
	void checkEndtest();

	private:

};