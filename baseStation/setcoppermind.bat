@echo off

set DIRECTORY=H:/Benjamin/Temp
set DESTINATION=src

if exist "%DIRECTORY%" (
        xcopy /s "%DESTINATION%" "%DIRECTORY%/baseStation/src/*"
) else (
        echo "%DIRECTORY% does not exist."
        echo "is coppermind mounted?"
)
