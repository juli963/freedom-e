#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/Sniffer_8b10b_40 "
        cd ../generated/Sniffer_8b10b_40
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VSniffer_8b10b_40__Syms.cpp obj_dir/VSniffer_8b10b_40__Trace__Slow.cpp obj_dir/VSniffer_8b10b_40__Trace.cpp obj_dir/VSniffer_8b10b_40.cpp usr/testbench.cpp usr/Sniffer_8b10b_40.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain device.hssniffer.p8b10b.Sniffer_8b10b_40 "
        echo "Switch Directory to generated/Sniffer_8b10b_40 "
        cd generated/Sniffer_8b10b_40
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc Sniffer_8b10b_40.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VSniffer_8b10b_40.mk
        echo "Switch Directory to generated/Sniffer_8b10b_40 "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VSniffer_8b10b_40__Syms.cpp obj_dir/VSniffer_8b10b_40__Trace__Slow.cpp obj_dir/VSniffer_8b10b_40__Trace.cpp obj_dir/VSniffer_8b10b_40.cpp usr/testbench.cpp usr/Sniffer_8b10b_40.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi