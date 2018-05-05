@echo off

set DIRECTORY=H:\Benjamin\Temp
set DESTINATION=src
set COPYPY=copy.py
set IGNORE=

if exist "%DIRECTORY%" (
    python %COPYPY% %DESTINATION% %DIRECTORY%\baseStation\%DESTINATION% False %IGNORE%
) else (
    echo %DIRECTORY% does not exist.
    echo is coppermind mounted?
)
