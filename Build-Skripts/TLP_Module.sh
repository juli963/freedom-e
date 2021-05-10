#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/TLP_Module "
        cd ../generated/TLP_Module
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VTLP_Module__Syms.cpp obj_dir/VTLP_Module__Trace__Slow.cpp obj_dir/VTLP_Module_PCIe_Scrambler_16_single.cpp obj_dir/VTLP_Module_LCRC_2_1D.cpp obj_dir/VTLP_Module__Trace.cpp obj_dir/VTLP_Module.cpp usr/testbench.cpp usr/TLP_Module.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain TLP.TLP_Module "
        echo "Switch Directory to generated/TLP_Module "
        cd generated/TLP_Module
        echo "Run Verilator ..."
        verilator -Wall --trace -cc TLP_Module.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VTLP_Module.mk
        echo "Switch Directory to generated/TLP_Module "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VTLP_Module__Syms.cpp obj_dir/VTLP_Module__Trace__Slow.cpp obj_dir/VTLP_Module_PCIe_Scrambler_16_single.cpp obj_dir/VTLP_Module_LCRC_2_1D.cpp obj_dir/VTLP_Module__Trace.cpp obj_dir/VTLP_Module.cpp usr/testbench.cpp usr/TLP_Module.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi