#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/NRZI_Sim_Top "
        cd ../generated/NRZI_Sim_Top
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VNRZI_Sim_Top__Syms.cpp obj_dir/VNRZI_Sim_Top__Trace__Slow.cpp obj_dir/VNRZI_Sim_Top__Trace.cpp obj_dir/VNRZI_Sim_Top.cpp usr/testbench.cpp usr/NRZI_Sim_Top.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain linecodes.NRZI.NRZI_Sim_Top "
        echo "Switch Directory to generated/NRZI_Sim_Top "
        cd generated/NRZI_Sim_Top
        echo "Run Verilator ..."
        verilator -Wall --trace -cc NRZI_Sim_Top.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VNRZI_Sim_Top.mk
        echo "Switch Directory to generated/NRZI_Sim_Top "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VNRZI_Sim_Top__Syms.cpp obj_dir/VNRZI_Sim_Top__Trace__Slow.cpp obj_dir/VNRZI_Sim_Top__Trace.cpp obj_dir/VNRZI_Sim_Top.cpp usr/testbench.cpp usr/NRZI_Sim_Top.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi