
# load stuff
import os
import argparse
import shutil

# parse cl args
parser = argparse.ArgumentParser( description="LibTile3D Builder" )
parser.add_argument( "--clean", help=f"(re-)generate build system", action="store_true" )
parser.add_argument( "--run", help=f"execute binary", action="store_true" )
args = parser.parse_args();

if args.clean:
    shutil.rmtree("build")
    
if not os.path.isdir("build"):
    print( "\nPreparing Target..." )
    os.mkdir("build")
    os.system("cd build && cmake ../")

print( "\nBuilding Target..." )
os.system("cd build && make")

if args.run:
    print( "\nRunning Target..." )
    if os.name == "nt":
        os.system("build/main.exe")
    else:
        os.system("build/main")
