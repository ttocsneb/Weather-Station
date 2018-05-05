DIRECTORY=~/coppermind/Benjamin/Temp
DESTINATION=src
COPYPY=copy.py
IGNORE=

if [ -d "$DIRECTORY" ]; then
	python3 $COPYPY $DESTINATION $DIRECTORY/baseStation/$DESTINATION False $IGNORE
else
	echo "$DIRECTORY does not exist."
	echo "is coppermind mounted?"
fi
