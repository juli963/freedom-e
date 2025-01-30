#if !defined(RANDOM_H)
    #define RANDOM_H 

    #include <stdint.h>
    #include <cstdlib>
    #include <ctime>

    uint32_t create_rand(uint32_t min, uint32_t max){       // Max nur bis, nicht einschließlich
            return min+ (rand() % max);
    }

    void init_rand(){
        srand(time(0));  // Initialize random number generator.  
    }
#endif