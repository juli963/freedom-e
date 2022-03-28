
#include "testbench.h"
#include "../obj_dir/VLitedram_test_model.h"
#include "stdint.h"

#if !defined(Litedram_test_model_H)
    #define Litedram_test_model_H 1
    class        Litedram_test_model_TB : public TESTBENCH<VLitedram_test_model> {
        public:
            void init();
        private:

    };
#endif
