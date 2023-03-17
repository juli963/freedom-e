#!/bin/bash

    trace=1
    if [ $1 = "t" ]; then
        trace=0
    fi
    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/RGMIIv2 "
        cd ../generated/RGMIIv2
        echo "Compile CPP Program ..."
        g++ -DTRACE=$trace -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VRGMIIv2__Syms.cpp obj_dir/VRGMIIv2__Trace__Slow.cpp obj_dir/VRGMIIv2__Trace.cpp obj_dir/VRGMIIv2.cpp usr/RGMII_PHY.cpp usr/PHY_Bus2.cpp usr/MII_PHY.cpp usr/testbench.cpp usr/RGMIIv2.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Ethernet.Interface.RGMII.RGMIIv2 "
        echo "Switch Directory to generated/RGMIIv2 "
        cd generated/RGMIIv2
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc RGMIIv2.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VRGMIIv2.mk
        echo "Switch Directory to generated/RGMIIv2 "
        cd ..
        echo "Compile CPP Program ..."
        g++ -DTRACE=$trace -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VRGMIIv2__Syms.cpp obj_dir/VRGMIIv2__Trace__Slow.cpp obj_dir/VRGMIIv2__Trace.cpp obj_dir/VRGMIIv2.cpp usr/RGMII_PHY.cpp usr/PHY_Bus2.cpp usr/MII_PHY.cpp usr/testbench.cpp usr/RGMIIv2.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    fi
    echo "$status"
    exit $status
    