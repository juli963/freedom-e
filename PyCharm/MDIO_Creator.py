import FileContents.cFiles
import os.path
from os import path

package = "Ethernet.Interface.MDIO"
mod_object = "MDIO"
rootDir = "generated"

def write_File(fpath, name, ext, content):
    if (not path.exists(fpath+"//"+name+"."+ext)):
        f = open(fpath+"//"+name+"."+ext, "w")
        f.write(content)
        f.close()

def create_Directory_Tree(rootDir):
    print("Create Directories\n")
    if(not os.path.exists("..//"+rootDir)):
        os.mkdir("..//"+rootDir)
        print("Create 'generated'\n")
    if(not os.path.exists("..//Build-Skripts")):
        os.mkdir("..//Build-Skripts")
        print("Create 'Build-Skripts'\n")
    if(not os.path.exists("..//"+rootDir+"//"+mod_object)):
        os.mkdir("..//"+rootDir+"//"+mod_object)
        print("Create 'generated/Module'\n")
    if(not os.path.exists("..//"+rootDir+"//"+mod_object+"//"+"usr")):
        os.mkdir("..//"+rootDir+"//"+mod_object+"//"+"usr")
        print("Create 'usr'\n")

def replace_content(content):
    content = str.replace(content, "MOD", mod_object)
    return content

def create_C_Files():
    print("Create User CPP Files\n")
    write_File("..//"+rootDir+"//"+mod_object+"//"+"usr", mod_object, "h", replace_content(FileContents.cFiles.mod_h))
    write_File("..//"+rootDir+"//"+mod_object+"//"+"usr", mod_object, "cpp", replace_content(FileContents.cFiles.mod_c))
    write_File("..//"+rootDir+"//"+mod_object+"//"+"usr", "testbench", "h", replace_content(FileContents.cFiles.tb_h))
    write_File("..//"+rootDir+"//"+mod_object+"//"+"usr", "testbench", "cpp", replace_content(FileContents.cFiles.tb_c))
    write_File("..//"+rootDir+"//"+mod_object+"//"+"usr", "Console_Defines", "h", replace_content(FileContents.cFiles.console_defs))

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
        status=$?
    else
        echo "Switch Directory to Root"
        cd ..
        echo "Generate Verilog ..."
        java -jar rocket-chip/sbt-launch.jar ++2.12.4 "runMain """+ package+ "." + mod_object + """ "
        echo "Switch Directory to """ + rootDir +"/" + mod_object + """ "
        cd """ +rootDir+ "/" + mod_object + """
        echo "Run Verilator ..."
        verilator -Wall -Wno-UNUSED -Wno-DECLFILENAME --trace -cc """ + mod_object + """.v
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
        status=$?
    fi"""
    write_File("..//Build-Skripts", mod_object,"sh", content)

create_Directory_Tree(rootDir)
create_C_Files()
create_Build_Skript()
