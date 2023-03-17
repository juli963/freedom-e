#!/bin/bash

trace=1
if [ $1 = "t" ]; then
    trace=0
fi

create_verilog()
{
    echo "Generate Verilog ..."
    java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Helper.Big_to_small $1 $2"
    echo "Switch Directory to generated/Big_to_small "
    cd generated/Big_to_small
    echo "Run Verilator ..."
    verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc Big_to_small.v
    echo "Switch Directory to obj_dir"
    cd obj_dir
    echo "Make Library ..."
    make -f VBig_to_small.mk
    echo "Switch Directory to generated/Big_to_small "
    cd ..
}

compile_c()
{
    echo "Compile CPP Program ..."
    g++ -g -DDEF_Inwidth=$1 -DDEF_Outwidth=$2 -DDEBUG=$trace -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VBig_to_small__Syms.cpp obj_dir/VBig_to_small__Trace__Slow.cpp obj_dir/VBig_to_small__Trace.cpp obj_dir/VBig_to_small.cpp usr/testbench.cpp usr/Big_to_small.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
}

run_prog(){
    echo "Run CPP Testbench"
    ./usr/testbench.o
    stat=$?
    if [ $stat == 0 ] 
    then
        status = $stat
    fi
    cd ../..
}

echo "Switch Directory to Root"
cd ..
status=1


for i in 16 32 64  # Inputwidths
do 

    echo "Inputwidth $i Outputwidth 8"
    create_verilog $i 8
    compile_c $i 8
    run_prog
    
    if [[ $i -ge 32 ]] # 16Bit
    then 
        echo "Inputwidth $i Outputwidth 16"
        create_verilog $i 16
        compile_c $i 16
        run_prog
    fi 
    
    if [ $i -ge 64 ] # 32Bit
    then 
        echo "Inputwidth $i Outputwidth 32"
        create_verilog $i 32
        compile_c $i 32
        run_prog
    fi 


    : 'if [ $i -ge 125 ] # 125MHz 
    then 
        for x in 2 3  # Interfaces GMII RGMII
        do
            echo "RGMII"
            create_verilog $i 2
            compile_c $i 2
            run_prog
            echo "GMII"
            create_verilog $i 3
            compile_c $i 3
            run_prog
        done
    fi'
done

echo "$status"
exit $status