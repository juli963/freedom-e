#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/Litedram_test_model "
        cd ../generated/Litedram_test_model
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VLitedram_test_model__Syms.cpp obj_dir/VLitedram_test_model__Trace__Slow.cpp obj_dir/VLitedram_test_model__Trace.cpp obj_dir/VLitedram_test_model.cpp usr/testbench.cpp usr/Litedram_test_model.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Memory.DRAM.Litedram_test_model "
        echo "Switch Directory to generated/Litedram_test_model "
        cd generated/Litedram_test_model
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc Litedram_test_model.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VLitedram_test_model.mk
        echo "Switch Directory to generated/Litedram_test_model "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VLitedram_test_model__Syms.cpp obj_dir/VLitedram_test_model__Trace__Slow.cpp obj_dir/VLitedram_test_model__Trace.cpp obj_dir/VLitedram_test_model.cpp usr/testbench.cpp usr/Litedram_test_model.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    fi
echo "$status"
exit $status