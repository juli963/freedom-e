#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/TLP_w_Configuration "
        cd ../generated/TLP_w_Configuration
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VTLP_w_Configuration__Syms.cpp obj_dir/VTLP_w_Configuration__Trace__Slow.cpp obj_dir/VTLP_w_Configuration__Trace.cpp obj_dir/VTLP_w_Configuration_LCRC_2_1D.cpp obj_dir/VTLP_w_Configuration_PCIe_Scrambler_16_single.cpp obj_dir/VTLP_w_Configuration.cpp usr/testbench.cpp usr/TLP_w_Configuration.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain TLP.Cfg.TLP_w_Configuration "
        echo "Switch Directory to generated/TLP_w_Configuration "
        cd generated/TLP_w_Configuration
        echo "Run Verilator ..."
        verilator -Wall --trace -cc TLP_w_Configuration.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VTLP_w_Configuration.mk
        echo "Switch Directory to generated/TLP_w_Configuration "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VTLP_w_Configuration__Syms.cpp obj_dir/VTLP_w_Configuration__Trace__Slow.cpp obj_dir/VTLP_w_Configuration__Trace.cpp obj_dir/VTLP_w_Configuration_LCRC_2_1D.cpp obj_dir/VTLP_w_Configuration_PCIe_Scrambler_16_single.cpp obj_dir/VTLP_w_Configuration.cpp usr/testbench.cpp usr/TLP_w_Configuration.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi