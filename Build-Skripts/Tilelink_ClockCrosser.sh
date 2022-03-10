#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/Tilelink_ClockCrosser "
        cd ../generated/Tilelink_ClockCrosser
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VTilelink_ClockCrosser__Syms.cpp obj_dir/VTilelink_ClockCrosser__Trace__Slow.cpp obj_dir/VTilelink_ClockCrosser__Trace.cpp obj_dir/VTilelink_ClockCrosser.cpp usr/testbench.cpp usr/Tilelink_ClockCrosser.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain juli.blocks.tilelink.Tilelink_ClockCrosser "
        echo "Switch Directory to generated/Tilelink_ClockCrosser "
        cd generated/Tilelink_ClockCrosser
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc Tilelink_ClockCrosser.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VTilelink_ClockCrosser.mk
        echo "Switch Directory to generated/Tilelink_ClockCrosser "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VTilelink_ClockCrosser__Syms.cpp obj_dir/VTilelink_ClockCrosser__Trace__Slow.cpp obj_dir/VTilelink_ClockCrosser__Trace.cpp obj_dir/VTilelink_ClockCrosser.cpp usr/testbench.cpp usr/Tilelink_ClockCrosser.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi