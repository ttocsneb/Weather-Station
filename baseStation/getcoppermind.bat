@echo off

set DIRECTORY=H:\Benjamin\Temp
set DESTINATION=src

if exist "%DIRECTORY%" (
	if not exist "%DIRECTORY%\baseStation\src" (
		echo Nothing to copy
	) else (
		copy "%DIRECTORY%\baseStation\src\*" "%DESTINATION%"
		del /q "%DIRECTORY%\baseStation\*"
		FOR /D %%p IN ("%DIRECTORY%\baseStation\*.*") DO rmdir "%%p" /s /q
		rmdir "%DIRECTORY%\baseStation"
	)
) else (
    echo %DIRECTORY% does not exist.
    echo is coppermind mounted?
)
