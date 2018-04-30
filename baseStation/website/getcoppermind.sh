
DIRECTORY=~/coppermind/Benjamin/Temp
DESTINATION=./

if [ -d "$DIRECTORY" ]; then
	if [ ! -d "$DIRECTORY/baseStation" ]; then
		echo "No source to copy"
	else
		cp -r -v "$DIRECTORY/baseStation/html" "$DESTINATION"
		rm -r -v "$DIRECTORY/baseStation"
	fi
else
	echo "$DIRECTORY does not exist."
	echo "is coppermind mounted?"
fi
