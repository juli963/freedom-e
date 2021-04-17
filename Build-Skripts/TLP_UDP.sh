#!/bin/bash

if [ $1 = "c" ]; then
    echo "Switch Directory to generated/LTSSM_UDP_test"
    cd ../generated/LTSSM_UDP_test
    echo "Compile CPP Program ..."
    g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VLTSSM_UDP_test__Syms.cpp obj_dir/VLTSSM_UDP_test_LCRC_2_1D.cpp obj_dir/VLTSSM_UDP_test_PCIe_Scrambler_16_single.cpp obj_dir/VLTSSM_UDP_test__Trace__Slow.cpp obj_dir/VLTSSM_UDP_test__Trace.cpp obj_dir/VLTSSM_UDP_test.cpp usr/testbench.cpp usr/TCP_Bus.cpp usr/LTSSM_TCP_test.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
    echo "Run CPP Testbench"
    ./usr/testbench.o
else
    echo "Switch Directory to Root"
    cd ..
    echo "Generate Verilog ..."
    java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain TLP.LTSSM_UDP_test"
    echo "Switch Directory to generated/LTSSM_UDP_test"
    cd generated/LTSSM_UDP_test 
    echo "Run Verilator ..."
    verilator -Wall --trace -cc LTSSM_UDP_test.v
    echo "Switch Directory to obj_dir"
    cd obj_dir
    echo "Make Library ..."
    make -f VLTSSM_UDP_test.mk
    echo "Switch Directory to generated/LTSSM_UDP_test"
    cd ..
    echo "Compile CPP Program ..."
    g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VLTSSM_UDP_test__Syms.cpp obj_dir/VLTSSM_UDP_test_LCRC_2_1D.cpp obj_dir/VLTSSM_UDP_test_PCIe_Scrambler_16_single.cpp obj_dir/VLTSSM_UDP_test__Trace__Slow.cpp obj_dir/VLTSSM_UDP_test__Trace.cpp obj_dir/VLTSSM_UDP_test.cpp usr/testbench.cpp usr/TCP_Bus.cpp usr/LTSSM_TCP_test.cpp  /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
    echo "Run CPP Testbench"
    ./usr/testbench.o
fi