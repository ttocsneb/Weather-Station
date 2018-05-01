@echo off

set DIRECTORY=H:\Benjamin\Temp
set DESTINATION=html

if exist "%DIRECTORY%" (
	if not exist "%DIRECTORY%\baseStation\%DESTINATION%" (
		echo Nothing to copy
	) else (
		copy "%DIRECTORY%\baseStation\%DESTINATION%\*" "%DESTINATION%"
		del /q "%DIRECTORY%\baseStation\%DESTINATION%\*"
		FOR /D %%p IN ("%DIRECTORY%\baseStation\*.*") DO rmdir "%%p" /s /q
		rmdir "%DIRECTORY%\baseStation"
	)
) else (
    echo %DIRECTORY% does not exist.
    echo is coppermind mounted?
)
