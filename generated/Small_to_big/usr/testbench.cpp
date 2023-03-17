
#include "Console_Defines.h"
#include "Small_to_big.h"
#include <ctime>
#include <queue>
#include <inttypes.h>

#include <cstdlib>

#if !defined(DEF_Inwidth)
        #error "Bitwidth In not defined"
#endif

#if !defined(DEF_Outwidth)
        #error "Bitwidth Out not defined"
#endif

#define DEF_Inbytes DEF_Inwidth/8
#define DEF_Outbytes DEF_Outwidth/8
#define DEF_OutDivIn DEF_Outwidth/DEF_Inwidth

#define D_data_in(x) (uint8_t*) &tb->m_core->io_data_in_##x
#define D_strb_in(x) (uint8_t*) &tb->m_core->io_strb_in_##x
#define D_error_in(x) (uint8_t*) &tb->m_core->io_error_in_##x

#define D_data_out(x) (uint8_t*) &tb->m_core->io_data_out_##x
#define D_strb_out(x) (uint8_t*) &tb->m_core->io_strb_out_##x
#define D_error_out(x) (uint8_t*) &tb->m_core->io_error_out_##x

#define inmask (1<<DEF_Inwidth)-1
#define outmask (1<<DEF_Outwidth)-1

//using namespace std;
uint32_t create_rand(uint32_t min, uint32_t max);
void fill_fifos( std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid, bool withDebug = false );
void print_fifo(std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid );
void check_fifos(Small_to_big_TB *tb, std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid, 
                uint8_t** data_in,uint8_t** data_out, uint8_t** strb_in, uint8_t** strb_out, uint8_t** error_in, uint8_t** error_out,
                bool withDebug );

static bool error_n = true;

int main(int argc, char **argv) {
        srand(time(0));  // Initialize random number generator.  
        const uint16_t Divider = DEF_OutDivIn;
        const bool debug = false;

        Small_to_big_TB *tb = new Small_to_big_TB();

        uint8_t *data_in[DEF_Inbytes], *data_out[DEF_Outbytes];
        uint8_t *strb_in[DEF_Inbytes], *strb_out[DEF_Outbytes], *error_in[DEF_Inbytes], *error_out[DEF_Outbytes];
       
        #if DEF_Inbytes > 0
                data_in[0] = D_data_in(0);
                strb_in[0] = D_strb_in(0);
                error_in[0] = D_error_in(0);
        #endif
        #if DEF_Inbytes > 1
                data_in[1] =  D_data_in(1);
                strb_in[1] =  D_strb_in(1);
                error_in[1] =  D_error_in(1);
        #endif
        #if DEF_Inbytes > 2
                data_in[2] =  D_data_in(2);
                strb_in[2] =  D_strb_in(2);
                error_in[2] =  D_error_in(2);
        #endif
        #if DEF_Inbytes > 3
                data_in[3] =  D_data_in(3);
                strb_in[3] =  D_strb_in(3);
                error_in[3] =  D_error_in(3);
        #endif
        #if DEF_Inbytes > 4
                data_in[4] =  D_data_in(4);
                strb_in[4] =  D_strb_in(4);
                error_in[4] =  D_error_in(4);
        #endif
        #if DEF_Inbytes > 5
                data_in[5] =  D_data_in(5);
                strb_in[5] =  D_strb_in(5);
                error_in[5] =  D_error_in(5);
        #endif
        #if DEF_Inbytes > 6
                data_in[6] =  D_data_in(6);
                strb_in[6] =  D_strb_in(6);
                error_in[6] =  D_error_in(6);
        #endif
        #if DEF_Inbytes > 7
                data_in[7] =  D_data_in(7);
                strb_in[7] =  D_strb_in(7);
                error_in[7] =  D_error_in(7);
        #endif

        #if DEF_Outbytes > 0
                data_out[0] =  D_data_out(0);
                strb_out[0] =  D_strb_out(0);
                error_out[0] =  D_error_out(0);
        #endif
        #if DEF_Outbytes > 1
                data_out[1] =  D_data_out(1);
                strb_out[1] =  D_strb_out(1);
                error_out[1] =  D_error_out(1);
        #endif
        #if DEF_Outbytes > 2
                data_out[2] =  D_data_out(2);
                strb_out[2] =  D_strb_out(2);
                error_out[2] =  D_error_out(2);
        #endif
        #if DEF_Outbytes > 3
                data_out[3] =  D_data_out(3);
                strb_out[3] =  D_strb_out(3);
                error_out[3] =  D_error_out(3);
        #endif
        #if DEF_Outbytes > 4
                data_out[4] =  D_data_out(4);
                strb_out[4] =  D_strb_out(4);
                error_out[4] =  D_error_out(4);
        #endif
        #if DEF_Outbytes > 5
                data_out[5] =  D_data_out(5);
                strb_out[5] =  D_strb_out(5);
                error_out[5] =  D_error_out(5);
        #endif
        #if DEF_Outbytes > 6
                data_out[6] =  D_data_out(6);
                strb_out[6] =  D_strb_out(6);
                error_out[6] =  D_error_out(6);
        #endif
        #if DEF_Outbytes > 7
                data_out[7] =  D_data_out(7);
                strb_out[7] =  D_strb_out(7);
                error_out[7] =  D_error_out(7);
        #endif
        


        printf("Conversion Parameters: Inbytes=%i, Outbytes=%i, Divider(Out/In)=%i\n", DEF_Inbytes, DEF_Outbytes, DEF_OutDivIn);

        std::queue<uint64_t> input_data;
        std::queue<uint64_t> output_data;
        std::queue<uint8_t> input_strb;
        std::queue<uint8_t> output_strb;
        std::queue<uint8_t> input_error;
        std::queue<uint8_t> output_error;
        std::queue<uint8_t> input_valid;
        std::queue<uint8_t> output_valid;


        #if TRACE==1
                tb->opentrace("trace.vcd");
        #endif
        tb->init();

        fill_fifos( &input_data, &output_data, &input_strb, &output_strb, &input_error, &output_error, &input_valid, &output_valid, debug );
        check_fifos(tb, &input_data, &output_data, &input_strb, &output_strb, &input_error, &output_error, &input_valid, &output_valid, 
                        data_in, data_out, strb_in, strb_out, error_in, error_out,
                        debug );

        for(uint16_t i = 0; i< create_rand(10,100); i++){
                tb->tick();
        }

        fill_fifos( &input_data, &output_data, &input_strb, &output_strb, &input_error, &output_error, &input_valid, &output_valid, debug );
        check_fifos(tb, &input_data, &output_data, &input_strb, &output_strb, &input_error, &output_error, &input_valid, &output_valid, 
                        data_in, data_out, strb_in, strb_out, error_in, error_out,
                        debug );

        
        if(error_n){
                CGREEN
                printf("Test was successful \n");
                CDEFAULT
        }else{
                CRED
                printf("Test failed \n");
                CDEFAULT
        }
        return error_n;

}


uint32_t create_rand(uint32_t min, uint32_t max){       // Max nur bis, nicht einschließlich
        return min+ (rand() % max);
}

void check_fifos(Small_to_big_TB *tb, std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid, 
                uint8_t** data_in, uint8_t** data_out, uint8_t** strb_in, uint8_t** strb_out, uint8_t** error_in, uint8_t** error_out,
                bool withDebug ){

        uint16_t fifo_size = input_data->size();
        uint8_t z = 0, x = 0, y = 0;
        uint64_t d_in, s_in, err_in;
        uint8_t valid_ff = 0, val_out;

        uint64_t d_out, s_out, err_out ;

        const uint16_t Divider = DEF_OutDivIn;
        

        for(uint16_t i=0; i<(fifo_size*Divider)+10; i++){

                if( input_data->size() > 0 ){
                        d_in = input_data->front();
                        s_in =  input_strb->front();
                        err_in = input_error->front();

                        for(z = 0; z<DEF_Inbytes; z++){
                                *(data_in[z]) = d_in & 0xFF;
                                *(strb_in[z]) = s_in & 0x01;
                                *(error_in[z]) = err_in & 0x01;
                                d_in >>= 8;
                                s_in >>= 1;
                                err_in >>= 1;
                        }


                        tb->m_core->io_valid_in = input_valid->front();
                        input_data->pop();
                        input_strb->pop();
                        input_error->pop();
                        input_valid->pop();
                }else{
                        tb->m_core->io_valid_in = 0;
                }

                
                while(output_valid->front() == 0 && output_valid->size() > 0 ){
                        output_data->pop();
                        output_strb->pop();
                        output_error->pop();
                        output_valid->pop();   
                }

                if( tb->m_core->io_valid_out > 0 ){
                         d_out = output_data->front(); 
                         s_out = output_strb->front();
                         err_out = output_error->front();
                         val_out = output_valid->front();
                        if( output_valid->size() > 0){
                                output_data->pop();
                                output_strb->pop();
                                output_error->pop();
                                output_valid->pop();
                        }
                }

                for(x = 0; x < DEF_Outbytes;x++){
                        if( tb->m_core->io_valid_out > 0 ){
                                if(withDebug){
                                        printf("Data @idx i=%i @Elem x=%i Data should: 0x%02X Data is: 0x%02X || Strb should: %i Strb is: %i || Err should: %i Err is: %i  \n", 
                                                i, x, (uint8_t)(d_out & 0xFF), *(data_out[x]), (uint8_t)(s_out & 0x01), *(strb_out[x]), (uint8_t)(err_out & 0x01), *(error_out[x]));
                                }
                                if(*(data_out[x]) != (d_out & 0xFF) ){
                                        CRED
                                        printf("Fail Data @idx i=%i @Elem x=%i Data should: 0x%02X Data is: 0x%02X \n", i, x, (uint8_t)(d_out & 0xFF), *(data_out[x]));
                                        CDEFAULT
                                        error_n = false;
                                        //exit;
                                }
                                if(*(strb_out[x]) != (s_out & 0x01)){
                                        CRED
                                        printf("Fail Strobe @idx i=%i @Elem x=%i Data should: %i Data is: %i \n", i, x, (uint8_t)(s_out & 0x01), *(strb_out[x]));
                                        CDEFAULT
                                        error_n = false;
                                        //exit;
                                }
                                if(*(error_out[x]) != (err_out & 0x01)){
                                        CRED
                                        printf("Fail Error @idx i=%i @Elem x=%i Data should: %i Data is: %i \n", i, x, (uint8_t)(err_out & 0x01), *(error_out[x]));
                                        CDEFAULT
                                        error_n = false;
                                        //exit;
                                }
                                if(tb->m_core->io_valid_out != val_out){
                                        CRED
                                        printf("Fail Valid @idx i=%i @Elem x=%i Data should: %i Data is: %i \n", i, x, val_out, tb->m_core->io_valid_out);
                                        CDEFAULT
                                        error_n = false;
                                        //exit;
                                }
                                d_out >>= 8;
                                s_out >>= 1;
                                err_out >>= 1;
                        }
                }
                tb->tick();


        }
        


}

void fill_fifos( std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid, bool withDebug ){

        uint8_t temp8 = 0;
        uint64_t temp64_in, temp64_out = 0;
        uint8_t x,cnt= 0;
        const uint16_t bitmask_in = ((1<<DEF_Inbytes)-1);
        for(uint16_t i = 0; i<create_rand(10,20); i++){
                temp64_in = 0;
                temp64_out = 0;
                for(x= 0; x<DEF_Outbytes; x++){
                        cnt++;
                        
                        //temp64_out >>= 8;
                        temp64_out >>= 8;
                        temp8 = create_rand(0,255);
                        temp64_out |= ((uint64_t)temp8<<(DEF_Outwidth-8));
                        temp64_in |= ((uint64_t)temp8<<(DEF_Inwidth));
                        temp64_in >>= 8;
                        //temp64_in >>= 8;
                        if(cnt >= DEF_Inbytes ){
                                
                                input_data->push(temp64_in);
                                temp64_in = 0;
                                cnt = 0;
                        }
                        
                }
                output_data->push(temp64_out);

                temp8 = create_rand(0,2); // Valid
                output_valid->push(temp8);
                for(x = 0; x<DEF_OutDivIn; x++){
                        input_valid->push(temp8);
                }

                temp64_in = 0;
                temp64_out = 0;

                for(x = 0; x<DEF_OutDivIn; x++){
                        temp64_in = create_rand(0,(1<<DEF_Inbytes)); // Error
                        input_error->push(temp64_in);
                        temp64_out |= ((temp64_in& bitmask_in )<<(DEF_Outbytes)); 
                        temp64_out >>= DEF_Inbytes; 
                }
                output_error->push(temp64_out);

                
                temp64_in = 0;
                temp64_out = 0;
                for(x = 0; x<DEF_OutDivIn; x++){
                        temp64_in = create_rand(0,(1<<DEF_Inbytes)); // Strobes
                        input_strb->push(temp64_in);
                        temp64_out |= ((temp64_in& bitmask_in )<<(DEF_Outbytes)); 
                        temp64_out >>= DEF_Inbytes; 
                }
                output_strb->push(temp64_out);
        }
        if(withDebug){
                print_fifo(input_data, output_data, input_strb, output_strb, input_error, output_error, input_valid, output_valid );
        }
        

}

void print_fifo(std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid ){

        printf("Output Data \tInput Data \tOutput Strb \tInput Strb \tOutput Error \tInput Error \tOutput Valid \tInput Valid\n");
        uint8_t cnt = DEF_OutDivIn;
        uint16_t fifo_size = input_data->size();
        for(uint16_t i=0; i<fifo_size; i++){
                cnt++;
                if( cnt >= DEF_OutDivIn ){
                        printf("0x%08lX \t0x%08lX \t0x%02X \t\t0x%02X \t\t0x%02X \t\t0x%02X \t\t0x%02X \t\t0x%02X \n", 
                        output_data->front(), input_data->front(), output_strb->front(), input_strb->front(), output_error->front(), input_error->front(), output_valid->front(), input_valid->front());
                        
                        output_data->push(output_data->front());
                        output_strb->push(output_strb->front());
                        output_error->push(output_error->front());
                        output_valid->push(output_valid->front());

                        output_data->pop();
                        output_strb->pop();
                        output_error->pop();
                        output_valid->pop();


                        cnt = 0;
                }else{
                        printf("\t\t0x%08lX \t\t\t0x%02X \t\t\t\t0x%02X \t\t\t\t0x%02X \n", 
                        input_data->front(), input_strb->front(), input_error->front(), input_valid->front());
                }


                input_data->push(input_data->front());
                input_strb->push(input_strb->front());
                input_error->push(input_error->front());
                input_valid->push(input_valid->front());

                input_data->pop();
                input_strb->pop();
                input_error->pop();
                input_valid->pop();
                

        }

}