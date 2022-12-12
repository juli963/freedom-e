#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/InbandDecoder "
        cd ../generated/InbandDecoder
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VInbandDecoder__Syms.cpp obj_dir/VInbandDecoder__Trace__Slow.cpp obj_dir/VInbandDecoder__Trace.cpp obj_dir/VInbandDecoder.cpp usr/testbench.cpp usr/InbandDecoder.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Ethernet.Interface.InbandDecoder.InbandDecoder "
        echo "Switch Directory to generated/InbandDecoder "
        cd generated/InbandDecoder
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc InbandDecoder.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VInbandDecoder.mk
        echo "Switch Directory to generated/InbandDecoder "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VInbandDecoder__Syms.cpp obj_dir/VInbandDecoder__Trace__Slow.cpp obj_dir/VInbandDecoder__Trace.cpp obj_dir/VInbandDecoder.cpp usr/testbench.cpp usr/InbandDecoder.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    fi
echo "$status"
exit $status