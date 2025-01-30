#!/bin/bash

trace=1
if [ $1 = "t" ]; then
    trace=0
fi

create_verilog()
{
    echo "Generate Verilog ..."
    java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain Ethernet.Interface.ClockCreator.ETHClock $1 $2"
    echo "Switch Directory to generated/ETHClock "
    cd generated/ETHClock
    echo "Run Verilator ..."
    verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc ETHClock.v
    echo "Switch Directory to obj_dir"
    cd obj_dir
    echo "Make Library ..."
    make -f VETHClock.mk
    echo "Switch Directory to generated/ETHClock "
    cd ..
}

compile_c()
{
    echo "Compile CPP Program ..."
    g++ -DTRACE=$trace -DCLK=$1 -DInterface=$2 -Iobj_dir -I../include -Iusr -I/usr/share/verilator/include obj_dir/VETHClock__Syms.cpp obj_dir/VETHClock__Trace__Slow.cpp obj_dir/VETHClock__Trace.cpp obj_dir/VETHClock.cpp usr/testbench.cpp usr/ETHClock.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
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

#for i in 25 50 75 100 125 250  # Frequencys
#do
#  echo "Looping ... number $i"
   #for i in 0 1 2 3  # Interfaces
   #do
   #done
#done

echo "Switch Directory to Root"
cd ..
status=1

#create_verilog 250 3
#cd generated/ETHClock
#compile_c 250 3
#run_prog



for i in 25 50 100 125 250  # Frequencys
do 
    if [[ $i -ge 25 && $i -ne 125 ]]  # 25MHz
    then
        echo "MII"
        create_verilog $i 0
        compile_c $i 0
        run_prog
    fi
    
    if [[ $i -ge 50 && $i -ne 125 && $i -ne 250 ]] # 50MHz
    then 
        echo "RMII"
        create_verilog $i 1
        compile_c $i 1
        run_prog
    fi 
    
    if [ $i -ge 125 ] # 125MHz
    then 
        echo "RGMII"
        create_verilog $i 2
        compile_c $i 2
        run_prog
    fi 

    if [ $i -ge 250 ] # 250MHz
    then 
        echo "GMII"
        create_verilog $i 3
        compile_c $i 3
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