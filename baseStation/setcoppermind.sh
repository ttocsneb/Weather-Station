DIRECTORY=~/coppermind/Benjamin/Temp
DESTINATION=src

if [ -d "$DIRECTORY" ]; then
	if [ ! -d "$DIRECTORY/baseStation" ]; then
		mkdir "$DIRECTORY/baseStation"
	fi
	cp -r "$DESTINATION" "$DIRECTORY/baseStation" 
else
	echo "$DIRECTORY does not exist."
	echo "is coppermind mounted?"
fi
