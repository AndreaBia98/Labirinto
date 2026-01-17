@echo off
setlocal
rem Nome eseguibile del progetto
set "EXE_NAME=Labirinto_struct.exe"

rem Cerca l'eseguibile nella cartella corrente e sottocartelle
set "EXE_PATH="
for /r "%~dp0" %%f in ("%EXE_NAME%") do (
  set "EXE_PATH=%%~ff"
  goto :found
)

echo Eseguibile "%EXE_NAME%" non trovato nel pacchetto.
pause
exit /b 1

:found
echo Avvio: "%EXE_PATH%"
"%EXE_PATH%"
endlocal