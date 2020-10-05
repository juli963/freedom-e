import glob
import os
import shutil
import sys
import string

path = "../sifive-blocks/src/main/scala/util/LFSR.scala"
moduleName = "FibonacciLFSR"
genName = "hni.blocks.util.mFibonacci"
verilatorPath = "../generated/LFSR"
c_srcs = "usr/LFSR.cpp"

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


def extract_LFSRParams(param):
    param = param_splitter(param)
    print(param)
    width = ""
    taps = []
    init = ""
    i = 0
    for p in param:
        ps = p.split("=")
        if ps[0] == "width" or i == 0:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            width = temp
        elif ps[0] == "taps" or i == 1:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            temp = temp.replace("Set(","")
            temp = temp.replace(")","")
            temp = temp.split(",")

            taps = temp
        elif ps[0] == "init" or i == 2:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            init = temp

        i = i+1

    return {
            "width" : width,
            "taps"  : taps,
            "init"  : init
            }

def write_LFSRconfig(params):
    file = open(verilatorPath+"/usr/config.h", "w")
    file.write("#include <stdint.h> \n\n")

    file.write("#if !defined(CONSTANTS_H)\n")
    file.write("#define CONSTANTS_H 1\n")

    file.write("const uint32_t LFSR_shifts = 1000;\n")
    file.write("const uint8_t LFSR_width = "+ params['width'] + ";\n")
    file.write("uint64_t LFSR_init = "+ params['init'] + ";\n")
    line = ""
    for p in params['taps']:
        line = line + "," + p
    line = line.strip(",")
    file.write("const uint8_t LFSR_taps[] = {" + line + "};\n")

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
            line = line[1:len(line)-2]  ## Cut parantheses
            extracted = extract_LFSRParams(line)
    file.close()

    write_LFSRconfig(extracted)

    os.system('cd .. && java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain '+genName+'"')
    if os.path.isdir(verilatorPath +"/obj_dir"):
        shutil.rmtree(verilatorPath +"/obj_dir")
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
