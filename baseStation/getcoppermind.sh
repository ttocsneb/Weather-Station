
DIRECTORY=~/coppermind/Benjamin/Temp
DESTINATION=./

if [ -d "$DIRECTORY" ]; then
	if [ ! -d "$DIRECTORY/baseStation" ]; then
		echo "No source to copy"
	else
		cp -r "$DIRECTORY/baseStation/src" "$DESTINATION"
		rm -r "$DIRECTORY/baseStation"
	fi
else
	echo "$DIRECTORY does not exist."
	echo "is coppermind mounted?"
fi
