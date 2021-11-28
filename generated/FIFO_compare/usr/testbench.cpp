
#include "Console_Defines.h"
#include "FIFO_compare.h"

struct FIFO_Interface {
  uint8_t* enq_ready;
  uint8_t* enq_valid;
  uint32_t* enq_data;

  uint8_t* deq_ready;
  uint8_t* deq_valid;
  uint32_t* deq_data;

  uint8_t* count;

} ;
void FIFO_test(FIFO_Interface FIFO, FIFO_compare_TB* tb, uint32_t size);

int main(int argc, char **argv) {

        FIFO_compare_TB *tb = new FIFO_compare_TB();

        tb->opentrace("trace.vcd");
        tb->init();

        FIFO_Interface chisel;
        chisel.enq_ready = &tb->m_core->io_chisel_fifo_enq_ready ;
        chisel.enq_valid = &tb->m_core->io_chisel_fifo_enq_valid ;
        chisel.enq_data = &tb->m_core->io_chisel_fifo_enq_bits ;
        chisel.deq_ready = &tb->m_core->io_chisel_fifo_deq_ready ;
        chisel.deq_valid = &tb->m_core->io_chisel_fifo_deq_valid ;
        chisel.deq_data = &tb->m_core->io_chisel_fifo_deq_bits ;
        chisel.count = &tb->m_core->io_chisel_fifo_count ;
        printf("Status: FIFO Test Chisel Implementation started\n"); 
        FIFO_test(chisel, tb, 4);
        tb->init();
        
        FIFO_Interface own;
        own.enq_ready = &tb->m_core->io_own_fifo_enq_ready ;
        own.enq_valid = &tb->m_core->io_own_fifo_enq_valid ;
        own.enq_data = &tb->m_core->io_own_fifo_enq_bits ;
        own.deq_ready = &tb->m_core->io_own_fifo_deq_ready ;
        own.deq_valid = &tb->m_core->io_own_fifo_deq_valid ;
        own.deq_data = &tb->m_core->io_own_fifo_deq_bits ;
        own.count = &tb->m_core->io_own_fifo_count ;

        printf("Status: FIFO Test Own Implementation started\n"); 
        FIFO_test(own, tb, 4);
        tb->init();
}

void FIFO_test(FIFO_Interface FIFO, FIFO_compare_TB* tb, uint32_t size){
        uint32_t testdata[size]; 
        uint32_t counter = 0;
        for (uint32_t i = 0; i < size; i++)
        {
              testdata[i] = i+1;  
        }

        if(*FIFO.enq_ready == 0){
                CRED
                printf("Error: FIFO Enqueue after reset not Ready (FIFO Full)\n"); 
                CDEFAULT
        }
        if(*FIFO.deq_valid != 0){
                CRED
                printf("Error: FIFO Dequeue after reset not Valid (FIFO not Empty)\n"); 
                CDEFAULT
        }
        while(*FIFO.enq_ready != 0){
                *FIFO.enq_data = testdata[counter];
                *FIFO.enq_valid = 1;
                counter++;
                if (counter >= size){
                        counter = 0;
                }
                tb->tick();
        }
        printf("\t Status: FIFO Full @ %i Counts\n", counter); 
        // Do one more write change with increased Data
        *FIFO.enq_data = *FIFO.enq_data + 1;
        for(uint8_t i = 0; i<100;i++){
                tb->tick();
        }

        // Check Dequeue Data
        for (uint32_t i = 0; i < size; i++)
        {
                if(*FIFO.enq_ready != 0){
                        *FIFO.enq_data = *FIFO.enq_data + 1;
                }
                *FIFO.deq_ready = 1;
                tb->tick();
        }


        *FIFO.enq_valid = 0;
        *FIFO.deq_ready = 0;
}

