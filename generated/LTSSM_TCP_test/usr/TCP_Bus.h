#if !defined(PCIe_H)
   #define PCIe_H 1

    #include "LTSSM_TCP_test.h"
    #include <stdint.h>



    class TCP_Bus {
        LTSSM_TB	*m_core[2];
        //LTSSM_TB	*m_core2;
        

        public:
            void Connect(uint8_t requester, uint32_t destIP, uint64_t destMAC, uint16_t Port);
            void Prepare_Address(uint8_t requester, uint32_t IP, uint64_t MAC);
            void Send_Data(uint8_t requester, uint32_t destIP, uint64_t destMAC, uint16_t Port, uint32_t ack_number, uint32_t seq_number, uint8_t ack, uint8_t* data, uint16_t Length);
            void Send_TLP(uint8_t requester, uint8_t* data, uint8_t* charisk, uint16_t Length);
            void Send_TLP_Checksum(uint8_t requester, uint8_t* data, uint16_t Length);
            uint32_t CalculateTlpCRC(uint8_t* data, uint8_t len);
            void CrossPCIe();

            TCP_Bus(LTSSM_TB* core1, LTSSM_TB* core2) {
                m_core[0] = core1;
                m_core[1] = core2;
            };

            
        private:
            uint16_t CalcTCPChecksum(uint32_t destIP, uint32_t srcIP, uint16_t Length, uint8_t* data);
            uint16_t CalcIPChecksum(uint16_t Length, uint8_t* data);
    };

    
#endif