@echo off

set DIRECTORY=H:\Benjamin\Temp
set DESTINATION=html

if exist "%DIRECTORY%" (
    if not exist "%DIRECTORY%\baseStation\%DESTINATION%" (
        mkdir "%DIRECTORY%\baseStation\%DESTINATION%"
    )
    copy "%DESTINATION%\*" "%DIRECTORY%\baseStation\%DESTINATION%"
) else (
    echo %DIRECTORY% does not exist.
    echo is coppermind mounted?
)
