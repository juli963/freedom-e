#!/bin/bash

trace=1
if [ $1 = "t" ]; then
    trace=0
fi

create_verilog()
{
    echo "Generate Verilog ..."
    java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Helper.Small_to_big $1 $2"
    echo "Switch Directory to generated/Small_to_big "
    cd generated/Small_to_big
    echo "Run Verilator ..."
    verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc Small_to_big.v
    echo "Switch Directory to obj_dir"
    cd obj_dir
    echo "Make Library ..."
    make -f VSmall_to_big.mk
    echo "Switch Directory to generated/Small_to_big "
    cd ..
}

compile_c()
{
    echo "Compile CPP Program ..."
    g++ -g -DDEF_Inwidth=$1 -DDEF_Outwidth=$2 -DTRACE=$trace -Iobj_dir -I../include -Iusr -I/usr/share/verilator/include obj_dir/VSmall_to_big__Syms.cpp obj_dir/VSmall_to_big__Trace__Slow.cpp obj_dir/VSmall_to_big__Trace.cpp obj_dir/VSmall_to_big.cpp usr/testbench.cpp usr/Small_to_big.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
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


for i in 8 16 32  # Inputwidths
do 

    echo "Inputwidth $i Outputwidth 64"
    create_verilog $i 64
    compile_c $i 64
    run_prog
    
    if [[ $i -le 16 ]] # 32Bit
    then 
        echo "Inputwidth $i Outputwidth 32"
        create_verilog $i 32
        compile_c $i 32
        run_prog
    fi 
    
    if [ $i -le 8 ] # 16Bit
    then 
        echo "Inputwidth $i Outputwidth 16"
        create_verilog $i 16
        compile_c $i 16
        run_prog
    fi 

done

echo "$status"
exit $status