import os
import subprocess
#import numpy as np

#import sys


def runprog(Prog, arg=""):
    
    actualPath = os.path.dirname(os.path.abspath(__file__))
    print("Running Test: "+ Prog)
    f = actualPath + '/' + Prog
        #tests[i] = actualPath + '/' + tests[i]
        #rets.append(subprocess.run(f).returncode)
    #rets = (subprocess.run(f, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode)
    farg = [f, arg]
    rets = (subprocess.run(farg).returncode)
        
    if rets == 0:
        erg = "\033[31m unsuccessful"
        print("Test: "+ Prog + erg + "\033[39m")
        return 0
    else:
        erg = "\033[32m successful"
        print("Test: "+ Prog + erg + "\033[39m")
        return 1
        

def test_Sniffer_8b10b_40():
    assert runprog("Sniffer_8b10b_40.sh", "t") == 1

def test_Litedram_test_model():
    assert runprog("Litedram_test_model.sh", "t") == 1

def test_PRBS_Boolean_Switch():
    assert runprog("PRBS_Boolean_Switch.sh", "t") == 1

def test_MDIO():
    assert runprog("MDIO.sh", "t") == 1

def test_ETHClock():
    assert runprog("ETHClock.sh", "t") == 1

def test_InbandDecoder():
    assert runprog("InbandDecoder.sh", "t") == 1

def test_MII():
    assert runprog("MII.sh", "t") == 1

def test_RMII():
    assert runprog("RMII.sh", "t") == 1

def test_GMII():
    assert runprog("GMII.sh", "t") == 1

def test_RGMIIv2():
    assert runprog("RGMIIv2.sh", "t") == 1

def test_Big_to_small():
    assert runprog("Big_to_small.sh", "t") == 1

def test_Small_to_big():
    assert runprog("Small_to_big.sh", "t") == 1

def test_Remove_n_bytes():
    assert runprog("Remove_n_bytes.sh", "t") == 1

#def test_generic(Prog):
    #sys.stderr.write("ssss")
    #fprintf( stderr, "HELP!" );
#    assert runprog(Prog) == 1

#test_generic("Sniffer_8b10b_40.sh")
#test_generic("Litedram_test_model.sh")

#test_Sniffer_8b10b_40()
#test_Litedram_test_model()