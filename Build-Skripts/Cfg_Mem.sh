#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to generated/Cfg_Mem "
        cd ../generated/Cfg_Mem
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VCfg_Mem__Syms.cpp obj_dir/VCfg_Mem__Trace__Slow.cpp obj_dir/VCfg_Mem__Trace.cpp obj_dir/VCfg_Mem.cpp usr/testbench.cpp usr/Cfg_Mem.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain TLP.Cfg.Cfg_Mem "
        echo "Switch Directory to generated/Cfg_Mem "
        cd generated/Cfg_Mem
        echo "Run Verilator ..."
        verilator -Wall --trace -cc Cfg_Mem.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f VCfg_Mem.mk
        echo "Switch Directory to generated/Cfg_Mem "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VCfg_Mem__Syms.cpp obj_dir/VCfg_Mem__Trace__Slow.cpp obj_dir/VCfg_Mem__Trace.cpp obj_dir/VCfg_Mem.cpp usr/testbench.cpp usr/Cfg_Mem.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi