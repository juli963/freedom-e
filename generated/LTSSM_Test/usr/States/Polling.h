#if !defined(Polling_H)
    #define Polling_H 1

    #include "../PCIe_Types.h"
    #include "../PCIe_detectors.h"
    #include "../Console_Defines.h"
    #include "CommonState.h"
    #include "FSMStorage.h"
    #include <stdint.h>
    

    class Polling : public CommonStates{
         public:
            Polling(FSMStorage* store) : CommonStates(store){};

            void Work();

            ~Polling(void){};
    };  
#endif