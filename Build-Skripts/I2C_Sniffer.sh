#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/I2C_Sniffer "
        cd ../generated/I2C_Sniffer
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VI2C_Sniffer__Syms.cpp obj_dir/VI2C_Sniffer__Trace__Slow.cpp obj_dir/VI2C_Sniffer__Trace.cpp obj_dir/VI2C_Sniffer.cpp usr/testbench.cpp usr/I2C_Slave.cpp usr/I2C_Host.cpp usr/I2C_Sniffer.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain juli.blocks.devices.I2C_Sniffer.I2C_Sniffer "
        echo "Switch Directory to generated/I2C_Sniffer "
        cd generated/I2C_Sniffer
        echo "Run Verilator ..."
        verilator -Wall --trace -cc I2C_Sniffer.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VI2C_Sniffer.mk
        echo "Switch Directory to generated/I2C_Sniffer "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VI2C_Sniffer__Syms.cpp obj_dir/VI2C_Sniffer__Trace__Slow.cpp obj_dir/VI2C_Sniffer__Trace.cpp obj_dir/VI2C_Sniffer.cpp usr/testbench.cpp usr/I2C_Slave.cpp usr/I2C_Host.cpp usr/I2C_Sniffer.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi