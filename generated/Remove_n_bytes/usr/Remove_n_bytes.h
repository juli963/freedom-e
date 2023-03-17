
#include "testbench.h"
#include "../obj_dir/VRemove_n_bytes.h"
#include "stdint.h"

#if !defined(Remove_n_bytes_H)
    #define Remove_n_bytes_H 1
    class        Remove_n_bytes_TB : public TESTBENCH<VRemove_n_bytes> {
        public:
            void init();
        private:

    };
#endif
