

#if !defined(Detectors_H)
    #define Detectors_H 1

    #include <stdint.h>
    #include <cstdio>
    #include "PCIe_Types.h"
    #include "Console_Defines.h"

    class Detector {
        public: 
            bool is = false;
            bool is_cons = false;
            void Detect(uint16_t rxdata, uint8_t charisk, bool valid){
                CRED
                printf("Detect not Implemented \n");
                CDEFAULT
            };

            Detector() {};
            virtual ~Detector(void){};
    };
    
    class TS_Detector : public Detector{
         public:
            TS_Detector(){};
            bool is_TS1 = false;
            uint8_t state = 0;
            uint8_t byteCounter = 0;
            void Detect(uint16_t rxdata, uint8_t charisk, bool valid);
            struct PCIe_Types::TrainSet data;

            ~TS_Detector(void){};
    };

    class EIOS_Detector : public Detector{
         public:
            EIOS_Detector(){};
            uint8_t state = 0;
            void Detect(uint16_t rxdata, uint8_t charisk, bool valid);            

            ~EIOS_Detector(void){};
    };
    
    class EIEOS_Detector : public Detector{
         public:
            EIEOS_Detector(){};
            uint8_t ByteCounter = 0;
            uint8_t state = 0;
            void Detect(uint16_t rxdata, uint8_t charisk, bool valid);            

            ~EIEOS_Detector(void){};
    };



#endif