#!/bin/bash

    trace=1
    if [ $1 = "t" ]; then
        trace=0
    fi

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/Remove_n_bytes "
        cd ../generated/Remove_n_bytes
        echo "Compile CPP Program ..."
        g++ -DDEF_Width=8 -DDEF_N=6 -DTRACE=$trace -Iobj_dir -I../include -Iusr -I/usr/share/verilator/include obj_dir/VRemove_n_bytes__Syms.cpp obj_dir/VRemove_n_bytes__Trace__Slow.cpp obj_dir/VRemove_n_bytes__Trace.cpp obj_dir/VRemove_n_bytes.cpp usr/testbench.cpp usr/Remove_n_bytes.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Helper.Remove_n_bytes 8 6"
        echo "Switch Directory to generated/Remove_n_bytes "
        cd generated/Remove_n_bytes
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc Remove_n_bytes.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VRemove_n_bytes.mk
        echo "Switch Directory to generated/Remove_n_bytes "
        cd ..
        echo "Compile CPP Program ..."
        g++ -DDEF_Width=8 -DDEF_N=6 -DTRACE=$trace -Iobj_dir -I../include -Iusr -I/usr/share/verilator/include obj_dir/VRemove_n_bytes__Syms.cpp obj_dir/VRemove_n_bytes__Trace__Slow.cpp obj_dir/VRemove_n_bytes__Trace.cpp obj_dir/VRemove_n_bytes.cpp usr/testbench.cpp usr/Remove_n_bytes.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    fi
    echo "$status"
    exit $status
    