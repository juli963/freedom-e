#!/bin/bash

trace=1
if [ $1 = "t" ]; then
    trace=0
fi

create_verilog()
{
    echo "Generate Verilog ..."
    java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Helper.Remove_n_bytes $1 $2"
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
}

compile_c()
{
    echo "Compile CPP Program ..."
    g++ -g -DDEF_Width=$1 -DDEF_N=$2 -DTRACE=$trace -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/VRemove_n_bytes__Syms.cpp obj_dir/VRemove_n_bytes__Trace__Slow.cpp obj_dir/VRemove_n_bytes__Trace.cpp obj_dir/VRemove_n_bytes.cpp usr/testbench.cpp usr/Remove_n_bytes.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
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


for i in 8 16 32 64  # Inputwidths
do 

    for x in 1 2 3 4 6 7 9 11
    do
        echo "Inputwidth $i N $x"
        create_verilog $i $x
        compile_c $i $x
        run_prog
    done

done

echo "$status"
exit $status