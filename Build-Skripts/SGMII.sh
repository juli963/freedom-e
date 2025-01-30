#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/SGMII "
        cd ../generated/SGMII
        rm ./usr/testbench.o
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I../include -I/usr/share/verilator/include -DDatawidth=16 -DTRACE=1 ../include/net/bus/PHY_Bus2.cpp ../include/net/phy/SGMII_PHY.cpp obj_dir/VSGMII__Syms.cpp obj_dir/VSGMII__Trace__Slow.cpp obj_dir/VSGMII__Trace.cpp obj_dir/VSGMII.cpp usr/testbench.cpp usr/SGMII.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Ethernet.Interface.SGMII 16"
        echo "Switch Directory to generated/SGMII "
        cd generated/SGMII
        rm ./usr/testbench.o
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc SGMII.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VSGMII.mk
        echo "Switch Directory to generated/SGMII "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I../include -I/usr/share/verilator/include -DDatawidth=16 -DTRACE=1 ../include/net/bus/PHY_Bus2.cpp ../include/net/phy/SGMII_PHY.cpp obj_dir/VSGMII__Syms.cpp obj_dir/VSGMII__Trace__Slow.cpp obj_dir/VSGMII__Trace.cpp obj_dir/VSGMII.cpp usr/testbench.cpp usr/SGMII.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    fi
    echo "$status"
    exit $status
    