import FileContents.cFiles
import os.path
from os import path

package = "TLP"
mod_object = "LTSSM_UDP_test"
rootDir = "generated"


def write_File(fpath, name, ext, content):
    if (not path.exists(fpath+"\\"+name+"."+ext)):
        f = open(fpath+"\\"+name+"."+ext, "w")
        f.write(content)
        f.close()

def create_Directory_Tree(rootDir):
    print("Create Directories\n")
    os.mkdir("..\\"+rootDir)
    os.mkdir("..\\Build-Skripts")
    os.mkdir("..\\"+rootDir+"\\"+mod_object)
    os.mkdir("..\\"+rootDir+"\\"+mod_object+"\\"+"usr")

def replace_content(content):
    content = str.replace(content, "MOD", mod_object)
    return content

def create_C_Files():
    print("Create User CPP Files\n")
    write_File("..\\"+rootDir+"\\"+mod_object+"\\"+"usr", mod_object, "h", replace_content(FileContents.cFiles.mod_h))
    write_File("..\\"+rootDir+"\\"+mod_object+"\\"+"usr", mod_object, "cpp", replace_content(FileContents.cFiles.mod_c))
    write_File("..\\"+rootDir+"\\"+mod_object+"\\"+"usr", "testbench", "h", replace_content(FileContents.cFiles.tb_h))
    write_File("..\\"+rootDir+"\\"+mod_object+"\\"+"usr", "testbench", "cpp", replace_content(FileContents.cFiles.tb_c))
    write_File("..\\"+rootDir+"\\"+mod_object+"\\"+"usr", "Console_Defines", "h", replace_content(FileContents.cFiles.console_defs))

def create_Build_Skript():
    print("Create Custom Build Script\n")
    content = """#!/bin/bash

    if [ $1 = "c" ]; then
        echo "Switch Directory to """ + rootDir +"/" + mod_object + """ "
        cd ../""" + rootDir +"/" + mod_object + """
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/V""" + mod_object + """__Syms.cpp obj_dir/V""" + mod_object + """__Trace__Slow.cpp obj_dir/V""" + mod_object + """__Trace.cpp obj_dir/V""" + mod_object + """.cpp usr/testbench.cpp usr/""" + mod_object + """.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain """+ package+ "." + mod_object + """ "
        echo "Switch Directory to """ + rootDir +"/" + mod_object + """ "
        cd """ +rootDir+ "/" + mod_object + """
        echo "Run Verilator ..."
        verilator -Wall --trace -cc """ + mod_object + """.v
        echo "Switch Directory to obj_dir"
        cd obj_dir
        echo "Make Library ..."
        make -f V""" + mod_object + """.mk
        echo "Switch Directory to """ +rootDir+"/" + mod_object + """ "
        cd ..
        echo "Compile CPP Program ..."
        g++ -Iobj_dir -Iusr -I/usr/share/verilator/include obj_dir/V""" + mod_object + """__Syms.cpp obj_dir/V""" + mod_object + """__Trace__Slow.cpp obj_dir/V""" + mod_object + """__Trace.cpp obj_dir/V""" + mod_object + """.cpp usr/testbench.cpp usr/""" + mod_object + """.cpp /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp -o usr/testbench.o
        echo "Run CPP Testbench"
        ./usr/testbench.o
    fi"""
    write_File("..\\Build-Skripts", mod_object,"sh", content)

create_Directory_Tree(rootDir)
create_C_Files()
create_Build_Skript()