@echo off

set DIRECTORY=H:\Benjamin\Temp
set DESTINATION=src

if exist "%DIRECTORY%" (
    if not exist "%DIRECTORY%\baseStation\src" (
        mkdir "%DIRECTORY%\baseStation\src"
    )
    copy "%DESTINATION%\*" "%DIRECTORY%\baseStation\src"
) else (
    echo %DIRECTORY% does not exist.
    echo is coppermind mounted?
)
