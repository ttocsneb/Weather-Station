
DIRECTORY=~/coppermind/Benjamin/Temp
DESTINATION=html
COPYPY=../copy.py
IGNORE=startbootstrap-sb-admin

if [ -d "$DIRECTORY" ]; then
	if [ ! -d "$DIRECTORY/baseStation" ]; then
		echo "No source to copy"
	else
		python3 $COPYPY $DIRECTORY/baseStation/$DESTINATION $DESTINATION True $IGNORE
		rm -r "$DIRECTORY/baseStation"
	fi
else
	echo "$DIRECTORY does not exist."
	echo "is coppermind mounted?"
fi
