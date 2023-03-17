
#include "Console_Defines.h"
#include "Remove_n_bytes.h"
#include <ctime>
#include <queue>
#include <inttypes.h>
#include <bitset>
#include <cstdlib>

#if !defined(DEF_N)
        #error "Bitwidth In not defined"
#endif

#if !defined(DEF_Width)
        #error "Bitwidth Out not defined"
#endif

#define DEF_Inbytes DEF_Width/8

#define D_data_in(x) (uint8_t*) &tb->m_core->io_data_in_##x
#define D_strb_in(x) (uint8_t*) &tb->m_core->io_strb_in_##x
#define D_error_in(x) (uint8_t*) &tb->m_core->io_error_in_##x

#define D_data_out(x) (uint8_t*) &tb->m_core->io_data_out_##x
#define D_strb_out(x) (uint8_t*) &tb->m_core->io_strb_out_##x
#define D_error_out(x) (uint8_t*) &tb->m_core->io_error_out_##x

uint32_t create_rand(uint32_t min, uint32_t max);
void fill_fifos( std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid, bool withDebug = false );
void print_fifo(std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid );
void check_fifos(Remove_n_bytes_TB *tb, std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid, 
                uint8_t** data_in,uint8_t** data_out, uint8_t** strb_in, uint8_t** strb_out, uint8_t** error_in, uint8_t** error_out,
                bool withDebug );
//uint8_t BitCount(uint64_t u);

static bool error_n = true;

int main(int argc, char **argv) {

        srand(time(0));  // Initialize random number generator.  
        const bool debug = false;

        Remove_n_bytes_TB *tb = new Remove_n_bytes_TB();

        uint8_t *data_in[DEF_Inbytes], *data_out[DEF_Inbytes];
        uint8_t *strb_in[DEF_Inbytes], *strb_out[DEF_Inbytes], *error_in[DEF_Inbytes], *error_out[DEF_Inbytes];

        printf("Conversion Parameters: Width=%i, N=%i, \n", DEF_Width, DEF_N);

        std::queue<uint64_t> input_data;
        std::queue<uint64_t> output_data;
        std::queue<uint8_t> input_strb;
        std::queue<uint8_t> output_strb;
        std::queue<uint8_t> input_error;
        std::queue<uint8_t> output_error;
        std::queue<uint8_t> input_valid;
        std::queue<uint8_t> output_valid;

       
        #if DEF_Inbytes > 0
                data_in[0] = D_data_in(0);
                strb_in[0] = D_strb_in(0);
                error_in[0] = D_error_in(0);

                data_out[0] = D_data_out(0);
                strb_out[0] = D_strb_out(0);
                error_out[0] = D_error_out(0);
        #endif
        #if DEF_Inbytes > 1
                data_in[1] =  D_data_in(1);
                strb_in[1] =  D_strb_in(1);
                error_in[1] =  D_error_in(1);

                data_out[1] = D_data_out(1);
                strb_out[1] = D_strb_out(1);
                error_out[1] = D_error_out(1);
        #endif
        #if DEF_Inbytes > 2
                data_in[2] =  D_data_in(2);
                strb_in[2] =  D_strb_in(2);
                error_in[2] =  D_error_in(2);

                data_out[2] = D_data_out(2);
                strb_out[2] = D_strb_out(2);
                error_out[2] = D_error_out(2);
        #endif
        #if DEF_Inbytes > 3
                data_in[3] =  D_data_in(3);
                strb_in[3] =  D_strb_in(3);
                error_in[3] =  D_error_in(3);

                data_out[3] = D_data_out(3);
                strb_out[3] = D_strb_out(3);
                error_out[3] = D_error_out(3);
        #endif
        #if DEF_Inbytes > 4
                data_in[4] =  D_data_in(4);
                strb_in[4] =  D_strb_in(4);
                error_in[4] =  D_error_in(4);

                data_out[4] = D_data_out(4);
                strb_out[4] = D_strb_out(4);
                error_out[4] = D_error_out(4);
        #endif
        #if DEF_Inbytes > 5
                data_in[5] =  D_data_in(5);
                strb_in[5] =  D_strb_in(5);
                error_in[5] =  D_error_in(5);

                data_out[5] = D_data_out(5);
                strb_out[5] = D_strb_out(5);
                error_out[5] = D_error_out(5);
        #endif
        #if DEF_Inbytes > 6
                data_in[6] =  D_data_in(6);
                strb_in[6] =  D_strb_in(6);
                error_in[6] =  D_error_in(6);

                data_out[6] = D_data_out(6);
                strb_out[6] = D_strb_out(6);
                error_out[6] = D_error_out(6);
        #endif
        #if DEF_Inbytes > 7
                data_in[7] =  D_data_in(7);
                strb_in[7] =  D_strb_in(7);
                error_in[7] =  D_error_in(7);

                data_out[7] = D_data_out(7);
                strb_out[7] = D_strb_out(7);
                error_out[7] = D_error_out(7);
        #endif

        #if TRACE==1
                tb->opentrace("trace.vcd");
        #endif
        tb->init();


        for(uint8_t i = 0; i<create_rand(10,20); i++){
        //for(uint8_t i = 0; i<1; i++){
                fill_fifos( &input_data, &output_data, &input_strb, &output_strb, &input_error, &output_error, &input_valid, &output_valid, debug );
                check_fifos(tb, &input_data, &output_data, &input_strb, &output_strb, &input_error, &output_error, &input_valid, &output_valid, 
                                data_in, data_out, strb_in, strb_out, error_in, error_out,
                                debug );

                // Clear FIFOs
                std::queue<uint64_t>().swap(input_data) ;
                std::queue<uint64_t>().swap( output_data);
                std::queue<uint8_t>().swap( input_strb);
                std::queue<uint8_t>().swap( output_strb);
                std::queue<uint8_t>().swap( input_error);
                std::queue<uint8_t>().swap( output_error);
                std::queue<uint8_t>().swap( input_valid);
                std::queue<uint8_t>().swap( output_valid);
        }
        

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

void check_fifos(Remove_n_bytes_TB *tb, std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid, 
                uint8_t** data_in, uint8_t** data_out, uint8_t** strb_in, uint8_t** strb_out, uint8_t** error_in, uint8_t** error_out,
                bool withDebug ){

        uint16_t fifo_size = input_data->size();
        uint64_t count_ones_in = 0;        // counter for strobes
        uint64_t count_ones_out = 0;        // counter for strobes

        uint64_t d_in, s_in, err_in;
        uint64_t d_out, s_out, err_out ;
        uint8_t z = 0, x = 0, y = 0;
        uint8_t val_out;

        for(uint16_t i=0; i<(fifo_size); i++){

                if( input_data->size() > 0 ){
                        d_in = input_data->front();
                        s_in =  input_strb->front();
                        err_in = input_error->front();

                        for(z = 0; z<DEF_Inbytes; z++){
                                *(data_in[z]) = d_in & 0xFF;
                                *(strb_in[z]) = s_in & 0x01;
                                count_ones_in += *(strb_in[z]);
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

                for(x = 0; x < DEF_Inbytes;x++){
                        if( tb->m_core->io_valid_out > 0 ){
                                if( *(strb_out[x]) > 0 ){
                                        count_ones_out++;
                                }
                                //count_ones_out +=  & 0x01;
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
                                if(     *(strb_out[x]) != (s_out & 0x01) && tb->m_core->io_valid_in > 0 ){
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

        // Check strobe Counter 
        if(withDebug){
                printf("Ones in:%li \n", count_ones_in);
                printf("Ones out:%li \n", count_ones_out);
        }
        if((count_ones_in-count_ones_out) != DEF_N ){
                CRED
                printf("Fail Strobes Number of 1's at Input: %li , Output: %li \n", count_ones_in, count_ones_out);
                CDEFAULT
                error_n = false;
                //exit;
        }

        
}


void fill_fifos( std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid, bool withDebug ){
        uint8_t temp8 = 0;
        uint64_t temp64_in=0;
        uint8_t x,cnt= 0;
        const uint16_t bitmask_in = ((1<<DEF_Inbytes)-1);
        for(uint16_t i = 0; i<create_rand(10,20); i++){ // Some Data without strobe or else
                temp64_in = 0;
                for(uint8_t x= 0; x<8;x++){
                        temp8 = create_rand(0,255);
                        temp64_in |= ((uint64_t)temp8<<(DEF_Width-8));
                        temp64_in >>= 8;
                }
                input_data->push(temp64_in);
                output_data->push(temp64_in);

                input_strb->push(0);
                output_strb->push(0);

                input_error->push(0);
                output_error->push(0);

                input_valid->push(0);
                output_valid->push(0);
        }

        for(uint16_t i = 0; i<create_rand(10,20); i++){ // All data with Strobe = 0xFFFF FFFF FFFF FFFF
                temp64_in = 0;
                for(uint8_t x= 0; x<8;x++){
                        
                        temp8 = create_rand(0,255);
                        temp64_in |= ((uint64_t)temp8<<(DEF_Width-8));
                        temp64_in >>= 8;
                }
                input_data->push(temp64_in);
                output_data->push(temp64_in);


                input_strb->push(0xFF);
                output_strb->push(0xFF);

                temp8 = create_rand(0,2);
                input_error->push(temp8);
                output_error->push(temp8);

                input_valid->push(1);
                output_valid->push(1);
        }

        temp64_in = 0;
        for(uint8_t x= 0; x<8;x++){
                temp8 = create_rand(0,255);
                temp64_in |= ((uint64_t)temp8<<(DEF_Width-8));
                temp64_in >>= 8;
        }
        input_data->push(temp64_in);
        output_data->push(temp64_in);

        temp8 = create_rand(1,DEF_Inbytes+1);
        temp8 = (1<<temp8)-1;
        input_strb->push(temp8);
        output_strb->push(temp8);

        temp8 = create_rand(0,2);
        input_error->push(temp8);
        output_error->push(temp8);

        input_valid->push(1);
        output_valid->push(1);


        for(uint16_t i = 0; i<create_rand(10,20); i++){ // Some Data without strobe or else
                temp64_in = 0;
                for(uint8_t x= 0; x<8;x++){
                        temp8 = create_rand(0,255);
                        temp64_in |= ((uint64_t)temp8<<(DEF_Width-8));
                        temp64_in >>= 8;
                }
                input_data->push(temp64_in);
                output_data->push(temp64_in);

                input_strb->push(0);
                output_strb->push(0);

                input_error->push(0);
                output_error->push(0);

                input_valid->push(0);
                output_valid->push(0);
        }


        if(withDebug){
                print_fifo(input_data, output_data, input_strb, output_strb, input_error, output_error, input_valid, output_valid );
        }

}

void print_fifo(std::queue<uint64_t>* input_data, std::queue<uint64_t>* output_data, std::queue<uint8_t>* input_strb, std::queue<uint8_t>* output_strb, std::queue<uint8_t>* input_error, std::queue<uint8_t>* output_error, std::queue<uint8_t>* input_valid, std::queue<uint8_t>* output_valid ){

        printf("Output Data \tInput Data \tOutput Strb \tInput Strb \tOutput Error \tInput Error \tOutput Valid \tInput Valid\n");
        uint16_t fifo_size = input_data->size();
        for(uint16_t i=0; i<fifo_size; i++){

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
/*uint8_t BitCount(uint64_t u)
{
        uint64_t uCount;

        uCount = u - ((u >> 1) & 033333333333) - ((u >> 2) & 011111111111);
        return (uint8_t) (((uCount + (uCount >> 3)) & 030707070707) % 63);
}*/