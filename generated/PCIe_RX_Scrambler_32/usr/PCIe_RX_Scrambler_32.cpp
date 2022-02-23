#include "PCIe_RX_Scrambler_32.h"


void PCIe_RX_Scrambler_32_TB::init(){
    printf("Scrambler Module loaded.\n");
    reset();

        for (uint i = 0; i<32; i++){
                m_core->reset = 1;
                tick();
        }
        m_core->reset = 0;
    tick();
}

bool PCIe_RX_Scrambler_32_TB::sync_rx_fifo(){
        while(!rx_fifo.empty()){
            if (! (rx_fifo.front().data == 0xBC && rx_fifo.front().isk > 0 ) ){
                rx_fifo.pop();
            }else{
                return true;
            }
        }
        return false;
}

uint16_t PCIe_RX_Scrambler_32_TB::rx_length(){
        return rx_fifo.size();
}

void PCIe_RX_Scrambler_32_TB::Scramblertick(){
    //uint64_t data = p8b10b[0].data | (p8b10b[1].data << 8) | (p8b10b[2].data << 16) | (p8b10b[3].data << 24);
    //uint8_t isk = p8b10b[0].isk | (p8b10b[1].isk << 1) | (p8b10b[2].isk << 2) | (p8b10b[3].isk << 3);

    if(change_dat){
        uint32_t data;
        uint8_t isk;

        data = m_core->io_Buffer_data;
        isk = m_core->io_Buffer_charisk;

        uint8_t charmask = 1;
        uint32_t datamask = 0xFF;

        s8b10b v8b10b;
        for(uint8_t i = 0; i<4; i++){
            v8b10b.isk = (isk & charmask)>>(i);
            v8b10b.data = (data & datamask)>>(uint32_t)(i*8);
            //printf("%X\n", v8b10b.data);
            rx_fifo.push( v8b10b );
            datamask <<= 8;
            charmask <<= 1;
        }
    }
    //change_dat = !change_dat;
}

uint8_t PCIe_RX_Scrambler_32_TB::RunScrambler()
{
	uint8_t ret = 0;

	for(int j=0; j<8; j++)
	{
		bool b = (scrambler_val & 0x8000) ? true : false;
		ret >>= 1;

		if(b)
		{
			ret |= 0x80;
			scrambler_val ^= 0x1c;
		}
		scrambler_val = (scrambler_val << 1) | b;
	}

	return ret;
}

void PCIe_RX_Scrambler_32_TB::soft_scramble(uint8_t* data, uint8_t* isk, uint16_t length, std::ofstream* myfile, uint8_t* data_scrambled, uint8_t* isk_scrambled){
    uint16_t train_start = 0;
    bool train_true = true;

    char hex_string[3];
    bool locked = false;
        
    uint16_t idx = 0;

    *myfile << "{ ";

    for(uint16_t x = 0; x < length; x++){
        if( !locked ){
            if( (data[x] == 0xBC && isk[x] > 0) ){
                locked = true;
            }
        }
        if(locked){
            if( !(data[x] == 0x1C && isk[x] > 0) ) { // if no SKP, advance 
                scramxor = RunScrambler();
            }

            if(data[x] == 0xBC && isk[x] > 0){  // Reset Scrambler
                scrambler_val = 0xFFFF;
                train_true = true;
                // Check if Training Set
                if((x+16) < length){
                    for(uint8_t p = 6; p<11; p++){
                        if( isk[x+p] == 0 ){
                            if( data[x+p] != 0x4A && data[x+p] != 0x45 && data[x+p] != ~0x4A && data[x+p] != ~0x45 ){
                                train_true = false;
                            }
                        }
                    }

                    if(train_true){ // Training Set recognized
                        train_start = x+1;
                    }
                }
                
            }
    

            // Output Data
            uint8_t output = 0;
            if( (isk[x] > 0) || (train_start <= x && train_start > 0 && (train_start+14) >= x)){    // K Word or Train Set recognized
                output = data[x];
            }else {
                output = data[x] ^ scramxor;
            }
            data_scrambled[idx] = output;
            isk_scrambled[idx] = isk[x] ;
            idx++;

            // Write Data
            sprintf(hex_string, "%02X", output); 
            *myfile << std::to_string(isk[x]) << hex_string << " ";
        }

    }
    *myfile << "}" << std::endl;
}

void PCIe_RX_Scrambler_32_TB::enq_tx_fifo(uint8_t* data, uint8_t* isk, uint16_t length){
    //printf("Data to Fifo added. Length: %i \n", length);
    s8b10b v8b10b;
    for(uint16_t i = 0; i<length; i++){
        v8b10b.data = data[i];
        v8b10b.isk = isk[i];
        tx_fifo.push( v8b10b );
    }
}

void PCIe_RX_Scrambler_32_TB::set_8b10b(s8b10b* p8b10b){
    uint64_t data = p8b10b[0].data | (p8b10b[1].data << 8) | (p8b10b[2].data << 16) | (p8b10b[3].data << 24);
    uint8_t isk = p8b10b[0].isk | (p8b10b[1].isk << 1) | (p8b10b[2].isk << 2) | (p8b10b[3].isk << 3);


    m_core->io_PHY_data = data;
    m_core->io_PHY_charisk = isk;
}

void PCIe_RX_Scrambler_32_TB::deq_tx_fifo(){
    if(change_dat){

        
        s8b10b data[4];
        for(uint8_t i=0; i<4; i++){
            if(!tx_fifo.empty()){
                data[i] = tx_fifo.front();
                //isk[i] = rx_fifo.front().isk;
                tx_fifo.pop();
            }else{
                data[i].data = 0;
                data[i].isk = 0;
            }
        }
        set_8b10b(data);
    }
    
}
