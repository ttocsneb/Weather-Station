import os
import shutil
import sys	

def recursive_overwrite(src, dest, ignore=None):
	if os.path.isdir(src):
		if not os.path.isdir(dest):
			os.makedirs(dest)
		files = os.listdir(src)
		if ignore is not None:
			ignored = ignore
		else:
			ignored = set()
		for f in files:
			if f not in ignored:
				recursive_overwrite(os.path.join(src, f),
									os.path.join(dest, f),
									ignore)
	else:
		print(src + " -> " + dest)
		shutil.copyfile(src, dest)

def copy(src, dest, ignore=None):
	recursive_overwrite(src, dest, ignore)

def usage():
	print("usage: " + sys.argv[0] + " <src> <dst> [remove src=False]")

if len(sys.argv) < 3:
	usage()
	sys.exit(0)

if len(sys.argv) >= 4:
	if sys.argv[3] == "True":
		removeSrc = True
	else:
		removeSrc = False
else:
	removeSrc = False

ignore = set()

if len(sys.argv) >= 5:
	i = 4
	while i < len(sys.argv):
		ignore.add(sys.argv[i])
		print("ignoring: " + sys.argv[i])
		i += 1

copy(sys.argv[1], sys.argv[2], ignore)


if removeSrc:
	shutil.rmtree(sys.argv[1])
	print("Deleted " + sys.argv[1])