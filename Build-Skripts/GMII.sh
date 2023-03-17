#!/bin/bash
    trace=1
    if [ $1 = "t" ]; then
        trace=0
    fi
    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/GMII "
        cd ../generated/GMII
        echo "Compile CPP Program ..."
        g++ -DTRACE=$trace -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VGMII__Syms.cpp obj_dir/VGMII__Trace__Slow.cpp obj_dir/VGMII__Trace.cpp obj_dir/VGMII.cpp usr/testbench.cpp usr/GMII_PHY.cpp usr/PHY_Bus2.cpp usr/MII_PHY.cpp usr/GMII.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Ethernet.Interface.GMII.GMII "
        echo "Switch Directory to generated/GMII "
        cd generated/GMII
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc GMII.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VGMII.mk
        echo "Switch Directory to generated/GMII "
        cd ..
        echo "Compile CPP Program ..."
        g++ -DTRACE=$trace -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VGMII__Syms.cpp obj_dir/VGMII__Trace__Slow.cpp obj_dir/VGMII__Trace.cpp obj_dir/VGMII.cpp usr/testbench.cpp usr/GMII_PHY.cpp usr/PHY_Bus2.cpp usr/MII_PHY.cpp usr/GMII.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    fi
    echo "$status"
    exit $status
    