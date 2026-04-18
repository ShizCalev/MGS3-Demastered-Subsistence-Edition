@echo off

REM Skip everything if CI env var exists
if defined CI (
    echo CI environment detected. Skipping update.
    goto :EOF
)

set "DEST=G:\Steam\steamapps\common\MGS3\plugins\MGS3-Demastered-Subsistence-Edition.asi"
set "SRC=C:\Development\Git\MGS3-Demastered-Subsistence-Edition\ASI Mod\x64\Release\MGS3-Demastered-Subsistence-Edition.asi"

if exist "%DEST%" (
    echo Found existing ASI, updating...
    copy /Y "%SRC%" "%DEST%"
    echo Done.
) else (
    echo Target ASI not found. Nothing copied.
)

