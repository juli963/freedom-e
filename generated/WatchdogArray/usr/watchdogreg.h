#include "testbench.h"
#include "config.h"


#if !defined(Watchdog_TB_H)
	#define Watchdog_TB_H 1

	class	Watchdog_TB : public TESTBENCH<VWatchdogArray> {
		public:

			Watchdog_TB(void){
				PRBS_polynom = 0;
				for(uint8_t i = 0; i< (sizeof(WD_PRBS_taps)/sizeof(WD_PRBS_taps[0]));i++){
					PRBS_polynom |= (1<<(WD_PRBS_taps[i]-1));
				}
				for(uint8_t i = 0; i< WD_regWidth;i++){
					Key_Mask |= ((uint64_t)1<<i);
				}
			}

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
				uint32_t pulsewidth;
				uint32_t mux;
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
				reg_ctrl_feed,
				reg_ctrl_pulsewidth,
				reg_ctrl_mux
			};

			// Variables
			struct wd_unit sWD;
			uint32_t Key = WD_Key;

			// Functions
			uint32_t readReg(Register reg, uint8_t timer=0);
			void writeReg(Register reg,uint32_t data, uint8_t timer=0);
			uint8_t readInterrupt(uint8_t interrupt = 0);
			void resetInterrupt(uint8_t interrupt = 0);
			void resetCounter(uint8_t timer = 0);
			uint8_t readOutput(uint8_t output = 0);
			void disableWatchdog(uint8_t timer = 0);
			unsigned long configWatchdog(struct wd_unit *sWD,uint8_t timer=0);	// Return tickcount
			void updateStruct(struct wd_unit *sWD,uint8_t timer=0);
			void unlock(void);
			void lock(void);
			void calc_key(void);
			void writeInv(uint32_t data = 0);
			void init(void){};

		private:
			uint64_t PRBS_polynom = 0;
			uint64_t Key_Mask = 0;
	};
	
#endif