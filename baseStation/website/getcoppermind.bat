@echo off

set DIRECTORY=H:\Benjamin\Temp
set DESTINATION=html
set COPYPY=..\copy.py
set IGNORE=startbootstrap-sb-admin

if exist "%DIRECTORY%" (
	if not exist "%DIRECTORY%\baseStation\%DESTINATION%" (
		echo Nothing to copy
	) else (
		python %COPYPY% %DIRECTORY%\baseStation\%DESTINATION% %DESTINATION% True %IGNORE%
		rmdir /s /q %DIRECTORY%\baseStation\
	)
) else (
    echo %DIRECTORY% does not exist.
    echo is coppermind mounted?
)