#if !defined(PCIe_H)
   #define PCIe_H 1

    #include "LTSSM_test.h"
    #include "PCIe_Types.h"
    #include "PCIe_detectors.h"
    #include <stdint.h>
    #include "States/Polling.h"
    #include "States/FSMStorage.h"



    class PCIe_training {
        LTSSM_TB	*m_core;
        

        public:
            FSMStorage *storage;
            Polling *cPolling;  
            void device_linkinit();

            PCIe_training(LTSSM_TB* core) {
                m_core = core;
                storage = new FSMStorage(m_core);
                cPolling = new Polling(storage);
            };
        private:

    };

    
#endif