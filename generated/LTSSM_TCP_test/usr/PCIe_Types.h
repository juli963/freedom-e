

#if !defined(PCIe_Types_H)
    #define PCIe_Types_H 1
    #include <stdint.h>

class PCIe_Types{
        public:
            struct TrainSet{
                uint8_t num_link;
                uint8_t num_lane;
                uint8_t n_fts;
                uint8_t data_rate;
                uint8_t training_control;
            };

            enum LTSSM { Detect, Polling, Configuration, L0, L0s, L2, L1, Disabled, Hot_Reset, Loopback, Recovery };

            static const uint8_t COM = 0xBC;   // K28.5 Lane and Link initialization
            static const uint8_t STP = 0xFB;   // K27.7 Start TLP
            static const uint8_t SDP = 0x5C;   // K28.2 Start DLLP
            static const uint8_t END = 0xFD;   // K29.7 End TLP or DLLP
            static const uint8_t EDB = 0xFE;   // K30.7 End nullified TLP
            static const uint8_t PAD = 0xF7;   // K23.7 Framing and Lane Ordering negotiations
            static const uint8_t SKP = 0x1C;   // K28.0 Compensating different Bit Rates 
            static const uint8_t FTS = 0x3C;   // K28.1 Exit L0s to L0 LSSTM
            static const uint8_t IDL = 0x7C;   // K28.3 Electrical Idle 
            static const uint8_t EIE = 0xFC;   // K28.7 Reserved at 2.5GT/s Electrical Idle Exit
            static const uint8_t LIDL = 0x00;  // h00 Logical Idle Symbol
            static const uint8_t TS1_Ident = 0x4A; // D10.2 Identifier at End of TS1 Set
            static const uint8_t TS2_Ident = 0x45; // D5.2  Identifier at End of TS2 Set      
    };

#endif