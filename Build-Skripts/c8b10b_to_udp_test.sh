#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/c8b10b_to_udp_test "
        cd ../generated/c8b10b_to_udp_test
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/Vc8b10b_to_udp_test_c8b10b_to_DRAM.cpp obj_dir/Vc8b10b_to_udp_test__Syms.cpp obj_dir/Vc8b10b_to_udp_test__Trace__Slow.cpp obj_dir/Vc8b10b_to_udp_test__Trace.cpp obj_dir/Vc8b10b_to_udp_test.cpp usr/testbench.cpp usr/c8b10b_to_udp_test.cpp usr/Ethernet.cpp usr/Memory.cpp usr/Sniffer_8b10b_40.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain device.hssniffer.p8b10b.c8b10b_to_udp_test "
        echo "Switch Directory to generated/c8b10b_to_udp_test "
        cd generated/c8b10b_to_udp_test
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc c8b10b_to_udp_test.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f Vc8b10b_to_udp_test.mk
        echo "Switch Directory to generated/c8b10b_to_udp_test "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/Vc8b10b_to_udp_test_c8b10b_to_DRAM.cpp obj_dir/Vc8b10b_to_udp_test__Syms.cpp obj_dir/Vc8b10b_to_udp_test__Trace__Slow.cpp obj_dir/Vc8b10b_to_udp_test__Trace.cpp obj_dir/Vc8b10b_to_udp_test.cpp usr/testbench.cpp usr/c8b10b_to_udp_test.cpp usr/Ethernet.cpp usr/Memory.cpp usr/Sniffer_8b10b_40.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi