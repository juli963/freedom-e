import glob
import os
import shutil
import sys
import string

path = "../sifive-blocks/src/main/scala/devices/mockaon/hniWatchdogTimer.scala"
moduleName = "WatchdogTimer"
genName = "hni.blocks.devices.watchdog.mWatchdog"
verilatorPath = "../generated/Watchdog"
c_srcs = "usr/watchdogreg.cpp"

def param_splitter(param):
    p = []
    i = 0
    stop = 0
    inner = False
    for c in param:
        if c == ',' and not inner:
            p.append(param[stop:i])
            stop = i+1
        elif c == '(':
            inner = True
        elif c == ')':
            inner = False
        i = i+1
    p.append(param[stop:])
    return p


def extract_WDParams(param):
    param = param_splitter(param)
    print(param)
    countwidth = ""
    cmpwidth = ""
    mode = ""
    i = 0
    for p in param:
        ps = p.split("=")
        if ps[0] == "pcountWidth" or i == 0:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            countwidth = temp
        elif ps[0] == "pcmpWidth" or i == 1:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            cmpwidth = temp
        elif ps[0] == "pMode" or i == 2:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            mode = temp

        i = i+1

    return {
            "countWidth" : countwidth,
            "cmpWidth"  : cmpwidth,
            "mode"  : mode
            }

def write_WDconfig(params):
    file = open(verilatorPath+"/usr/config.h", "w")
    file.write("#include <stdint.h> \n\n")

    file.write("#if !defined(CONSTANTS_H)\n")
    file.write("#define CONSTANTS_H 1\n")

    if "both" in params['mode'].lower():
        file.write("#define MODE_BOTH\n")
    elif "window" in params['mode'].lower():
        file.write("#define MODE_WINDOW\n")
    else:
        file.write("#define MODE_TIMEOUT\n")

    file.write("const uint8_t WD_countWidth = "+ params['countWidth'] + ";\n")
    file.write("const uint8_t WD_cmpWidth = "+ params['cmpWidth'] + ";\n")

    file.write("#endif\n")
    file.close()

arg = ""
if len(sys.argv) > 1:
    arg = string.join(sys.argv[1:])

if arg != "c":
    file = open(path,"r")
    extracted = {}
    for line in file:
        if "chisel3.Driver.execute" in line:
            line = line.split(moduleName)
            line = moduleName.join(line[1:])
            line = line.replace(" ", "")
            line = line.replace("\r", "")
            line = line.replace("\n", "")
            line = line[1:len(line) - 2]  ## Cut parantheses
            extracted = extract_WDParams(line)
    file.close()

    write_WDconfig(extracted)

    os.system('cd .. && java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain '+genName+'"')
    if os.path.isdir(verilatorPath + "/obj_dir"):
        shutil.rmtree(verilatorPath + "/obj_dir")
    os.system('cd '+ verilatorPath +' && verilator -Wall --trace -cc '+ moduleName +'.v')
    os.system('cd '+ verilatorPath +'/obj_dir && make -f V'+ moduleName +'.mk')

verilatorFiles = " "
for cfile in glob.glob(verilatorPath +"/obj_dir/*.cpp"):
    if(not("ALL" in cfile)):
        cfile = cfile.replace(verilatorPath + "/", "")
        verilatorFiles = verilatorFiles + cfile + " "
print('cd '+ verilatorPath +' && g++ -Iobj_dir -Iusr -I/usr/share/verilator/include'+ verilatorFiles + c_srcs +' usr/testbench.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o\n' )
os.system('cd '+ verilatorPath +' && g++ -Iobj_dir -Iusr -I/usr/share/verilator/include'+ verilatorFiles + c_srcs +' usr/testbench.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o')
os.system('cd '+ verilatorPath +' && ./usr/testbench.o')

