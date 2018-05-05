@echo off

set DIRECTORY=H:\Benjamin\Temp
set DESTINATION=html
set COPYPY=..\copy.py
set IGNORE=startbootstrap-sb-admin

if exist "%DIRECTORY%" (
	python %COPYPY% %DESTINATION% %DIRECTORY%\baseStation\%DESTINATION% False %IGNORE%
	
) else (
    echo %DIRECTORY% does not exist.
    echo is coppermind mounted?
)
