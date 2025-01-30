#!/bin/bash

    trace=1
    if [ $1 = "t" ]; then
        trace=0
    fi
    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/RMII "
        cd ../generated/RMII
        echo "Compile CPP Program ..."
        g++ -DTRACE=$trace -Iobj_dir -I../include -Iusr -I/usr/share/verilator/include obj_dir/VRMII__Syms.cpp obj_dir/VRMII__Trace__Slow.cpp obj_dir/VRMII__Trace.cpp obj_dir/VRMII.cpp usr/PHY_Bus2.cpp usr/RMII_PHY.cpp usr/testbench.cpp usr/RMII.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Ethernet.Interface.RMII.RMII "
        echo "Switch Directory to generated/RMII "
        cd generated/RMII
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc RMII.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VRMII.mk
        echo "Switch Directory to generated/RMII "
        cd ..
        echo "Compile CPP Program ..."
        g++ -DTRACE=$trace -Iobj_dir -I../include -Iusr -I/usr/share/verilator/include obj_dir/VRMII__Syms.cpp obj_dir/VRMII__Trace__Slow.cpp obj_dir/VRMII__Trace.cpp obj_dir/VRMII.cpp usr/PHY_Bus2.cpp usr/RMII_PHY.cpp usr/testbench.cpp usr/RMII.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    fi
    echo "$status"
    exit $status
    