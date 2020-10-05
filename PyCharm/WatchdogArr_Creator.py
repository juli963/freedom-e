import glob
import os
import shutil
import sys
import string

path = "../sifive-blocks/src/main/scala/devices/mockaon/hniWatchdogArray.scala"
moduleName = "WatchdogArray"
genName = "hni.blocks.devices.watchdog.mWatchdogArray"
verilatorPath = "../generated/WatchdogArray"
c_srcs = "usr/watchdogreg.cpp usr/testroutine.cpp"

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


def extract_WDarrParams(param):
    param = param_splitter(param)
    print(param)
    Dogs = ""
    Resets = ""
    Ints = ""
    PulseWidth = ""
    Offset = ""
    PRBS = ""
    PRBS_Set = []
    Key = ""
    regWidth = ""
    countwidth = ""
    cmpwidth = ""
    mode = ""
    i = 0
    for p in param:
        ps = p.split("=")
        if ps[0] == "Dogs" or i == 0:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            Dogs = temp
        elif ps[0] == "Resets" or i == 1:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            Resets = temp
        elif ps[0] == "Ints" or i == 2:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            Ints = temp
        elif ps[0] == "Mode" or i == 3:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            mode = temp
        elif ps[0] == "PulseWidth" or i == 4:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            PulseWidth = temp
        elif ps[0] == "Offset" or i == 5:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            Offset = temp
        elif ps[0] == "PRBS" or i == 6:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            PRBS = temp#"true" in temp.lower()
        elif ps[0] == "PRBS_Set" or i == 7:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            temp = temp.replace("Set(", "")
            temp = temp.replace(")", "")
            temp = temp.split(",")
            PRBS_Set = temp
        elif ps[0] == "Key" or i == 8:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            Key = temp
        elif ps[0] == "regWidth" or i == 9:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            regWidth = temp
        elif ps[0] == "pcountWidth" or i == 10:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            countwidth = temp
        elif ps[0] == "pcmpWidth" or i == 11:
            if len(ps) > 1:
                temp = ps[1]
            else:
                temp = p
            cmpwidth = temp

        i = i+1

    return {
            "Dogs"      : Dogs,
            "Resets"    : Resets,
            "Ints"      : Ints,
            "PulseWidth": PulseWidth,
            "Offset"    : Offset,
            "PRBS"      : PRBS,
            "PRBS_Set"  : PRBS_Set,
            "Key"       : Key,
            "regWidth"  : regWidth,
            "countWidth": countwidth,
            "cmpWidth"  : cmpwidth,
            "mode"      : mode
            }


def write_WDarrconfig(params):
    print(params)
    file = open(verilatorPath+"/usr/config.h", "w")
    file.write("#include <stdint.h> \n\n")

    file.write("#if !defined(CONSTANTS_H)\n")
    file.write("\t#define CONSTANTS_H 1\n\n")

    if "both" in params['mode'].lower():
        file.write("\t#define MODE_BOTH\n")
    elif "window" in params['mode'].lower():
        file.write("\t#define MODE_WINDOW\n")
    else:
        file.write("\t#define MODE_TIMEOUT\n")



    file.write("\tconst uint8_t WD_Dogs = "+ params['Dogs'] + ";\n")
    file.write("\tconst uint8_t WD_Resets = "+ params['Resets'] + ";\n")
    file.write("\tconst uint8_t WD_Ints = "+ params['Ints'] + ";\n")
    file.write("\tconst uint64_t WD_Offset = "+ params['Offset'] + ";\n")
    file.write("\tconst bool WD_PRBS = "+ params['PRBS'] + ";\n")
    line = ""
    for p in params['PRBS_Set']:
        line = line + "," + p
    line = line.strip(",")
    file.write("\tconst uint8_t WD_PRBS_taps[] = {" + line + "};\n")
    file.write("\tconst uint32_t WD_Key = "+ params['Key'] + ";\n")
    file.write("\tconst uint8_t WD_regWidth = "+ params['regWidth'] + ";\n")
    file.write("\tconst uint8_t WD_countWidth = "+ params['countWidth'] + ";\n")
    file.write("\tconst uint8_t WD_cmpWidth = "+ params['cmpWidth'] + ";\n\n")

    # Write Defines for Read and Write Signals
    file.write("\t#define read_TIMER_REG(name,timer) \\\n")
    file.write("\t\treturn m_core->io_WDIO_##timer##_##name; \\\n")
    file.write("\t\tbreak; \n\n")

    file.write("\t#define read_CASE_Entry_TIMER_REG(name,id) \\\n")
    file.write("\t\tcase id: \\\n")
    file.write("\t\tread_TIMER_REG(name, id) \n\n")

    file.write("\t#define read_CASE_TIMER_REG(name) \\\n")
    file.write("\t\tswitch(timer) { ")
    for i in range(int(params['Dogs'])):
        file.write("\\\n")
        file.write("\t\t\tread_CASE_Entry_TIMER_REG(name, "+str(i)+") ")
    file.write("\\\n")
    file.write("\t\t} \n\n")

    file.write("\t#define write_TIMER_REG(name,timer) \\\n")
    file.write("\t\tm_core->io_WDIO_##timer##_##name = data; \n\n")

    file.write("\t#define write_TIMER_REG_valid(name,timer,value) \\\n")
    file.write("\t\tm_core->io_WDIO_##timer##_##name = value; \n\n")

    file.write("\t#define write_CASE_Entry_TIMER_REG(name,name_valid,id) \\\n")
    file.write("\t\tcase id: \\\n")
    file.write("\t\t\twrite_TIMER_REG_valid(name_valid, id, true) \\\n")
    file.write("\t\t\twrite_TIMER_REG(name, id) \\\n")
    file.write("\t\t\tbreak; \n\n")

    file.write("\t#define write_TIMER_REGS_valid(name,value) ")
    for i in range(int(params['Dogs'])):
        file.write("\\\n")
        file.write("\t\twrite_TIMER_REG_valid(name, "+str(i)+", value) ")
    file.write("\\\n\n")

    file.write("\t#define write_CASE_TIMER_REG(name,name_valid) \\\n")
    file.write("\t\tswitch(timer) { ")
    for i in range(int(params['Dogs'])):
        file.write("\\\n")
        file.write("\t\t\twrite_CASE_Entry_TIMER_REG(name, name_valid, "+str(i)+") ")
    file.write("\\\n")
    file.write("\t\t} \n\n")

    # Output and Interrupts

    file.write("\t#define read_Interrupt(channel) \\\n")
    file.write("\t\treturn m_core->io_interrupts_##channel; \\\n")
    file.write("\t\tbreak;\n\n")

    file.write("\t#define read_CASE_Entry_Interrupt(channel) \\\n")
    file.write("\t\tcase channel: \\\n")
    file.write("\t\t\tread_Interrupt(channel)\n\n")

    file.write("\t#define read_CASE_Interrupt \\\n")
    file.write("\t\tswitch(interrupt){ ")
    for i in range(int(params['Ints'])):
        file.write("\\\n")
        file.write("\t\t\tread_CASE_Entry_Interrupt("+str(i)+") ")
    file.write("\\\n")
    file.write("\t\t}\n\n")

    file.write("\t#define read_Output(channel) \\\n")
    file.write("\t\treturn m_core->io_outputs_##channel; \\\n")
    file.write("\t\tbreak;\n\n")

    file.write("\t#define read_CASE_Entry_Output(channel) \\\n")
    file.write("\t\tcase channel: \\\n")
    file.write("\t\t\tread_Output(channel)\n\n")

    file.write("\t#define read_CASE_Output \\\n")
    file.write("\t\tswitch(output){ ")
    for i in range(int(params['Resets'])):
        file.write("\\\n")
        file.write("\t\t\tread_CASE_Entry_Output("+str(i)+") ")
    file.write("\\\n")
    file.write("\t\t}\n\n")


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
            line = line.split("new " + moduleName)
            line = moduleName.join(line[1:])
            line = line.replace(" ", "")
            line = line.replace("\r", "")
            line = line.replace("\n", "")
            line = line[1:len(line) - 2]  ## Cut parantheses
            extracted = extract_WDarrParams(line)
    file.close()

    write_WDarrconfig(extracted)

    os.system('cd .. && java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain '+genName+'"')
    if os.path.isdir("../generated/"+ moduleName +"/obj_dir"):
        shutil.rmtree("../generated/"+ moduleName +"/obj_dir")
    os.system('cd '+ verilatorPath +' && verilator -Wall --trace -cc '+ moduleName +'.v')
    os.system('cd '+ verilatorPath +'/obj_dir && make -f V'+ moduleName +'.mk')

verilatorFiles = " "
for cfile in glob.glob("../generated/"+ moduleName +"/obj_dir/*.cpp"):
    if(not("ALL" in cfile)):
        cfile = cfile.replace("../generated/"+ moduleName + "/", "")
        verilatorFiles = verilatorFiles + cfile + " "
print('cd '+ verilatorPath +' && g++ -Iobj_dir -Iusr -I/usr/share/verilator/include'+ verilatorFiles + c_srcs +' usr/testbench.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o\n' )
os.system('cd '+ verilatorPath +' && g++ -Iobj_dir -Iusr -I/usr/share/verilator/include'+ verilatorFiles + c_srcs +' usr/testbench.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o')
os.system('cd '+ verilatorPath +' && ./usr/testbench.o')

