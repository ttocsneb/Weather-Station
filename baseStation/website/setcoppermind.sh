DIRECTORY=~/coppermind/Benjamin/Temp
DESTINATION=html

if [ -d "$DIRECTORY" ]; then
	if [ ! -d "$DIRECTORY/baseStation" ]; then
		mkdir "$DIRECTORY/baseStation"
	fi
	cp -r -v "$DESTINATION" "$DIRECTORY/baseStation" 
else
	echo "$DIRECTORY does not exist."
	echo "is coppermind mounted?"
fi
