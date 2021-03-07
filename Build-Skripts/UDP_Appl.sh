#!/bin/bash

if [ $1 = "c" ]; then
    echo "Switch Directory to generated/UDP_Appl_Sim"
    cd ../generated/UDP_Appl_Sim
    echo "Compile CPP Program ..."
    g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VUDP_Appl_Sim__Syms.cpp obj_dir/VUDP_Appl_Sim__Trace__Slow.cpp obj_dir/VUDP_Appl_Sim__Trace.cpp obj_dir/VUDP_Appl_Sim.cpp usr/testbench.cpp usr/UDP_Appl_test.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
    echo "Run CPP Testbench"
    ./usr/testbench.o
else
    echo "Switch Directory to Root"
    cd ..
    echo "Generate Verilog ..."
    java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Ethernet.Protocol.ApplUDP_ACK.UDP_Appl_Sim"
    echo "Switch Directory to generated/UDP_Appl_Sim"
    cd generated/UDP_Appl_Sim 
    echo "Run Verilator ..."
    verilator -Wall --trace -cc UDP_Appl_Sim.v
    echo "Switch Directory to obj_dir"
    cd obj_dir
    echo "Make Library ..."
    make -f VUDP_Appl_Sim.mk
    echo "Switch Directory to generated/UDP_Appl_Sim"
    cd ..
    echo "Compile CPP Program ..."
    g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VUDP_Appl_Sim__Syms.cpp obj_dir/VUDP_Appl_Sim__Trace__Slow.cpp obj_dir/VUDP_Appl_Sim__Trace.cpp obj_dir/VUDP_Appl_Sim.cpp usr/testbench.cpp usr/UDP_Appl_test.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
    echo "Run CPP Testbench"
    ./usr/testbench.o
fi
