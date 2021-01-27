#!/bin/bash

if [ $1 = "c" ]; then
    echo "Switch Directory to generated/LTSSM_Test"
    cd ../generated/LTSSM_Test
    echo "Compile CPP Program ..."
    g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VLTSSM_test__Syms.cpp obj_dir/VLTSSM_test__Trace__Slow.cpp obj_dir/VLTSSM_test__Trace.cpp obj_dir/VLTSSM_test.cpp usr/testbench.cpp usr/LTSSM_test.cpp usr/PCIe_detectors.cpp usr/States/FSMStorage.cpp usr/States/Polling.cpp usr/PCIe_training.cpp  /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
    echo "Run CPP Testbench"
    ./usr/testbench.o
else
    echo "Switch Directory to Root"
    cd ..
    echo "Generate Verilog ..."
    java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain PCIe.LTSSM_test"
    echo "Switch Directory to generated/LTSSM_Test"
    cd generated/LTSSM_Test 
    echo "Run Verilator ..."
    verilator -Wall --trace -cc LTSSM_test.v
    echo "Switch Directory to obj_dir"
    cd obj_dir
    echo "Make Library ..."
    make -f VLTSSM_test.mk
    echo "Switch Directory to generated/LTSSM_Test"
    cd ..
    echo "Compile CPP Program ..."
    g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VLTSSM_test__Syms.cpp obj_dir/VLTSSM_test__Trace__Slow.cpp obj_dir/VLTSSM_test__Trace.cpp obj_dir/VLTSSM_test.cpp usr/testbench.cpp usr/LTSSM_test.cpp usr/PCIe_detectors.cpp usr/PCIe_training.cpp usr/States/FSMStorage.cpp usr/States/Polling.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
    echo "Run CPP Testbench"
    ./usr/testbench.o
fi