#!/usr/bin/env python3

# load stuff
import os
import argparse
import shutil

# parse cl args
parser = argparse.ArgumentParser( description="LibTile3D Builder" )
parser.add_argument( "--clean", help=f"(re-)generate build system", action="store_true" )
parser.add_argument( "--run", help=f"execute binary", action="store_true" )
parser.add_argument( "--debug", help=f"to build in debug mode", action="store_true" )
args = parser.parse_args();

main = "build/main" + (".exe" if os.name == "nt" else "")
options = "-DCMAKE_BUILD_TYPE=Debug" if args.debug else ""

if args.clean:
	shutil.rmtree("build")
    
if not os.path.isdir("build"):
	print( "\nPreparing Target..." )
	os.mkdir("build")
	os.system(f"cd build && cmake {options} ../")

print( "\nBuilding Target..." )
try:
	os.remove(main)
except:
	pass
os.system("cd build && make -j 4")

if args.run:
	print( "\nRunning Target..." )
	print( f"Executable: '{main}'" )
	os.system(main)
