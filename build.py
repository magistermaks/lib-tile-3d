#!/usr/bin/env python3

# load stuff
import os
import argparse
import shutil

# parse cl args
parser = argparse.ArgumentParser(description="LibTile3D Builder")
parser.add_argument("--clean", help="(re-)generate build system", action="store_true")
parser.add_argument("--run", help="execute binary", action="store_true")
parser.add_argument("--debug", help="to build in debug mode", action="store_true")
parser.add_argument("--xansi", help="disable ANSI color codes", action="store_true")
args = parser.parse_args();

# used for clean logging
def write(msg):
	global args;
	if not args.xansi:
		print(f"\n\033[0;94m{msg}\033[00m")
	else:
		print(f"\n{msg}")

# add cmake debug mode flag when required
options = "-DCMAKE_BUILD_TYPE=Debug " if args.debug else ""

# set system specific constants
if os.name == "nt":
	main = "build/main.exe"
	make = "nmake"
	options += "-G \"NMake Makefiles\" "
else:
	main = "build/main"
	make = "make -j $(nproc)"

# remove buid dir when required
if args.clean:
	try:
		shutil.rmtree("build")
	except:
		pass

# call cmake
if not os.path.isdir("build"):
	write("Preparing Target...")
	os.mkdir("build")
	os.system(f"cd build && cmake {options} ../")

write("Building Target...")
try:
	os.remove(main)
except:
	pass

# build project
os.system(f"cd build && {make}")

# run project when required
if args.run:
	print(f"Executable: './{main}'")
	write("Running Target...")
	os.system(main)

