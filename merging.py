import shutil
import os

project = "nexys4base"
builddir = "Nexys4-base"
filename = "Makefile."+project
# Using readlines()
file1 = open(filename, 'r')
Lines = file1.readlines()
 
count = 0
# Strips the newline character
trig = False

if not os.path.exists("builds/"+builddir+"/include/"):
    os.makedirs("builds/"+builddir+"/include/")

for line in Lines:
    count += 1
    if "vsrcs" in line.lower(): 
        trig = True
        line = line.split( ":=" )[1]
    if trig:
        #print("Line{}: {}".format(count, line.strip()))

        splits = line.replace("\\", " ").split(" ")

        #for loop to iterate over words array
        for split in splits:
            splitline = split.strip()
            splitline = splitline.replace("$(FPGA_DIR)","fpga-shells/xilinx")
            splitline = splitline.replace("$(sifiveblocks_dir)","sifive-blocks")
            splitline = splitline.replace("$(juliblocks_dir)","juli-blocks")
            splitline = splitline.replace("$(rocketchip_dir)","rocket-chip")
            if(splitline != "") and "$(BUILD_DIR)" not in splitline:
                #print("File: "+splitline)
                fname_split = splitline.split("/")
                fname = fname_split[len(fname_split)-1]
                shutil.copyfile(splitline, "builds/"+builddir+"/include/"+fname)

        if "\\" not in line: 
            trig = False

main_src = ""
file_srcs = []
for file in os.listdir("builds/"+builddir):
    if file.endswith(".v") and not (".rom." in file.lower()):
        main_src = os.path.join("builds/"+builddir, file)
    if file.endswith(".v") and (".rom." in file.lower()):
        file_srcs.append( os.path.join("builds/"+builddir, file) )

for file in os.listdir("builds/"+builddir+"/include/"):
    if file.endswith(".v"):
        file_srcs.append(os.path.join("builds/"+builddir+"/include/", file))
print(main_src)

main_file = open(main_src, 'a')
if main_src != "" and len(file_srcs) > 0:
    for obj in file_srcs:
        print(obj)
        file2 = open(obj, 'r')
        Lines = file2.readlines()
        main_file.write('\n\n')
        main_file.write('//-----------------------------------')
        main_file.write('//  '+obj)
        main_file.write('//-----------------------------------')
        for line in Lines:
            main_file.write(line)



main_file.close()