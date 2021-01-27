#if !defined(FSMStorage_H)
    #define FSMStorage_H 1

    #include "../PCIe_Types.h"
    #include "../PCIe_detectors.h"
    #include "../Console_Defines.h"
    #include "../LTSSM_test.h"
    
    #include <stdint.h>
    

    class FSMStorage{
        LTSSM_TB	*m_core;
        public:
        FSMStorage(LTSSM_TB *core) {m_core = core;};

        PCIe_Types::LTSSM state = PCIe_Types::Polling;

        TS_Detector *det_TS = new TS_Detector[1];
        EIOS_Detector *det_EIOS = new EIOS_Detector[1];
        EIEOS_Detector *det_EIEOS = new EIEOS_Detector[1];

        void send_idle(uint8_t n);
        void send_rand(uint8_t n);
        void send_skp(uint8_t n, uint8_t pos);
        void send_ts1();
        void send_ts1(struct PCIe_Types::TrainSet link_data, uint8_t w_skp = 0);
        void send_ts2();
        void send_ts2(struct PCIe_Types::TrainSet link_data, uint8_t w_skp = 0);


        ~FSMStorage(void){};
    };  
#endif