#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/PRBS_Boolean_Switch "
        cd ../generated/PRBS_Boolean_Switch
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -I../include -Iusr -I/usr/share/verilator/include obj_dir/VPRBS_Boolean_Switch__Syms.cpp obj_dir/VPRBS_Boolean_Switch__Trace__Slow.cpp obj_dir/VPRBS_Boolean_Switch__Trace.cpp obj_dir/VPRBS_Boolean_Switch.cpp usr/testbench.cpp usr/PRBS_Boolean_Switch.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Helper.PRBS_Boolean_Switch "
        echo "Switch Directory to generated/PRBS_Boolean_Switch "
        cd generated/PRBS_Boolean_Switch
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc PRBS_Boolean_Switch.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VPRBS_Boolean_Switch.mk
        echo "Switch Directory to generated/PRBS_Boolean_Switch "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -I../include -Iusr -I/usr/share/verilator/include obj_dir/VPRBS_Boolean_Switch__Syms.cpp obj_dir/VPRBS_Boolean_Switch__Trace__Slow.cpp obj_dir/VPRBS_Boolean_Switch__Trace.cpp obj_dir/VPRBS_Boolean_Switch.cpp usr/testbench.cpp usr/PRBS_Boolean_Switch.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
        status=$?
    fi
echo "$status"
exit $status