@echo off
title Membuka Orien Browser (Portable Mode)...
cd /d "%~dp0"

if exist "out\Release\orien.exe" (
    start "" "out\Release\orien.exe" --user-data-dir="%~dp0User_Data" %*
) else if exist "orien.exe" (
    start "" "orien.exe" --user-data-dir="%~dp0User_Data" %*
) else if exist "out\Release\brave.exe" (
    start "" "out\Release\brave.exe" --user-data-dir="%~dp0User_Data" %*
) else if exist "brave.exe" (
    start "" "brave.exe" --user-data-dir="%~dp0User_Data" %*
) else (
    echo [ERROR] File executable Orien tidak ditemukan!
    echo Pastikan proses kompilasi sudah selesai di folder out\Release.
    pause
)
