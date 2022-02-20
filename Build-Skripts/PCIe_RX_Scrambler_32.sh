#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/PCIe_RX_Scrambler_32 "
        cd ../generated/PCIe_RX_Scrambler_32
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VPCIe_RX_Scrambler_32__Syms.cpp obj_dir/VPCIe_RX_Scrambler_32__Trace__Slow.cpp obj_dir/VPCIe_RX_Scrambler_32__Trace.cpp obj_dir/VPCIe_RX_Scrambler_32.cpp usr/testbench.cpp usr/PCIe_RX_Scrambler_32.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain PCIe.Buffer.Scrambler.PCIe_RX_Scrambler_32 "
        echo "Switch Directory to generated/PCIe_RX_Scrambler_32 "
        cd generated/PCIe_RX_Scrambler_32
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc PCIe_RX_Scrambler_32.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VPCIe_RX_Scrambler_32.mk
        echo "Switch Directory to generated/PCIe_RX_Scrambler_32 "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VPCIe_RX_Scrambler_32__Syms.cpp obj_dir/VPCIe_RX_Scrambler_32__Trace__Slow.cpp obj_dir/VPCIe_RX_Scrambler_32__Trace.cpp obj_dir/VPCIe_RX_Scrambler_32.cpp usr/testbench.cpp usr/PCIe_RX_Scrambler_32.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi