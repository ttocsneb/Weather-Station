DIRECTORY=~/coppermind/Benjamin/Temp
DESTINATION=html
COPYPY=../copy.py
IGNORE=startbootstrap-sb-admin

if [ -d "$DIRECTORY" ]; then
	python3 $COPYPY $DESTINATION $DIRECTORY/baseStation/$DESTINATION False $IGNORE
else
	echo "$DIRECTORY does not exist."
	echo "is coppermind mounted?"
fi
