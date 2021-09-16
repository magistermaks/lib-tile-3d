#!/usr/bin/env python3

# import stuff
import os
import argparse
import shutil

# parse cl args
parser = argparse.ArgumentParser(description="LibTile3D Builder")
parser.add_argument("--clean", help="(re-)generate build system", action="store_true")
parser.add_argument("--run", help="execute given target", nargs='?', default=None, const="world")
parser.add_argument("--debug", help="build in debug mode", action="store_true")
parser.add_argument("--xansi", help="disable ANSI colors", action="store_true")
parser.add_argument("--list", help="list available targets", action="store_true")
args = parser.parse_args();

# used for clean logging
def write(msg):
	global args;
	if not args.xansi:
		print(f"\n\033[0;94m{msg}\033[00m")
	else:
		print(f"\n{msg}")

# list available targets when requesed
if args.list:
	print("Available targets:")
	path = "./demo"

	exclude_files = lambda x : not os.path.isfile(os.path.join(path,x))

	for file in filter(exclude_files, os.listdir(path)):
		print( " * --run=" + file )

	exit()

# add cmake debug mode flag when requesed
options = ("-DCMAKE_BUILD_TYPE=Debug " if args.debug else "")

# set system specific constants
if os.name == "nt":
	main = "main.exe"
	make = "nmake"
	options += "-G \"NMake Makefiles\" "
else:
	main = "main"
	make = "make -j ${nproc}"

# remove buid dir when requesed
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
if args.run is not None:
	binary = f"build/demo/{args.run}/{main}"

	print(f"Executable: './{binary}'")
	write("Running Target...")
	os.system(binary)

