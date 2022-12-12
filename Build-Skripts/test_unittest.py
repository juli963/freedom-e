import os
import subprocess
#import numpy as np

#import sys


def runprog(Prog):
    
    actualPath = os.path.dirname(os.path.abspath(__file__))
    print("Running Test: "+ Prog)
    f = actualPath + '/' + Prog
        #tests[i] = actualPath + '/' + tests[i]
        #rets.append(subprocess.run(f).returncode)
    #rets = (subprocess.run(f, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode)
    rets = (subprocess.run(f).returncode)
        
    if rets == 0:
        erg = "\033[31m unsuccessful"
        print("Test: "+ Prog + erg + "\033[39m")
        return 0
    else:
        erg = "\033[32m successful"
        print("Test: "+ Prog + erg + "\033[39m")
        return 1
        

def test_Sniffer_8b10b_40():
    assert runprog("Sniffer_8b10b_40.sh") == 1

def test_Litedram_test_model():
    assert runprog("Litedram_test_model.sh") == 1

def test_PRBS_Boolean_Switch():
    assert runprog("PRBS_Boolean_Switch.sh") == 1

def test_MDIO():
    assert runprog("MDIO.sh") == 1

def test_ETHClock():
    assert runprog("ETHClock.sh") == 1

def test_InbandDecoder():
    assert runprog("InbandDecoder.sh") == 1

#def test_generic(Prog):
    #sys.stderr.write("ssss")
    #fprintf( stderr, "HELP!" );
#    assert runprog(Prog) == 1

#test_generic("Sniffer_8b10b_40.sh")
#test_generic("Litedram_test_model.sh")

#test_Sniffer_8b10b_40()
#test_Litedram_test_model()