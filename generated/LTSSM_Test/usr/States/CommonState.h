



#if !defined(States_H)
    #define States_H 1

    #include "../PCIe_Types.h"
    #include "../PCIe_detectors.h"
    #include "../Console_Defines.h"
    #include "FSMStorage.h"
    #include <stdint.h>

    class CommonStates {
        protected:
            FSMStorage *m_store;
        public: 
            
            void Work(){
                CRED
                printf("Work not Implemented %i\n", m_store->state);
                CDEFAULT
            };

            CommonStates(FSMStorage *store) {m_store = store;};
            virtual ~CommonStates(void){};
    };
    
#endif