#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/ESP_FIFO_Test "
        cd ../generated/ESP_FIFO_Test
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VESP_FIFO_Test__Syms.cpp obj_dir/VESP_FIFO_Test__Trace__Slow.cpp obj_dir/VESP_FIFO_Test__Trace.cpp obj_dir/VESP_FIFO_Test.cpp usr/testbench.cpp usr/ESP_FIFO_Test.cpp usr/Ethernet.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain juli.blocks.devices.ESP_FIFO.ESP_FIFO_Test "
        echo "Switch Directory to generated/ESP_FIFO_Test "
        cd generated/ESP_FIFO_Test
        echo "Run Verilator ..."
        verilator -Wall --trace -cc ESP_FIFO_Test.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VESP_FIFO_Test.mk
        echo "Switch Directory to generated/ESP_FIFO_Test "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VESP_FIFO_Test__Syms.cpp obj_dir/VESP_FIFO_Test__Trace__Slow.cpp obj_dir/VESP_FIFO_Test__Trace.cpp obj_dir/VESP_FIFO_Test.cpp usr/testbench.cpp usr/ESP_FIFO_Test.cpp usr/Ethernet.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi