#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/FIFO_compare "
        cd ../generated/FIFO_compare
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VFIFO_compare__Syms.cpp obj_dir/VFIFO_compare__Trace__Slow.cpp obj_dir/VFIFO_compare__Trace.cpp obj_dir/VFIFO_compare.cpp usr/testbench.cpp usr/FIFO_compare.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain FIFO.FIFO_compare "
        echo "Switch Directory to generated/FIFO_compare "
        cd generated/FIFO_compare
        echo "Run Verilator ..."
        verilator -Wall --trace -cc FIFO_compare.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VFIFO_compare.mk
        echo "Switch Directory to generated/FIFO_compare "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VFIFO_compare__Syms.cpp obj_dir/VFIFO_compare__Trace__Slow.cpp obj_dir/VFIFO_compare__Trace.cpp obj_dir/VFIFO_compare.cpp usr/testbench.cpp usr/FIFO_compare.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi