
# load stuff
import os
import argparse
import shutil

# parse cl args
parser = argparse.ArgumentParser( description="LibTile3D Builder" )
parser.add_argument( "--clean", help=f"(re-)generate build system", action="store_true" )
parser.add_argument( "--run", help=f"execute binary", action="store_true" )
args = parser.parse_args();

main = "build/main" + (".exe" if os.name == "nt" else "")

if args.clean:
	shutil.rmtree("build")
    
if not os.path.isdir("build"):
	print( "\nPreparing Target..." )
	os.mkdir("build")
	os.system("cd build && cmake ../")

print( "\nBuilding Target..." )
os.remove(main)
os.system("cd build && make")

if args.run:
	print( "\nRunning Target..." )
	print( f"Executable: '{main}'" )
	os.system(main)
