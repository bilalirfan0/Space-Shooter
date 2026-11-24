@echo off
setlocal DisableDelayedExpansion
if not exist "%~dp0build\game.exe" (
    echo Missing build\game.exe. Extract the entire ZIP and try again.
    pause
    exit /b 1
)
pushd "%~dp0build" || exit /b 1
.\game.exe
set "gameExitCode=%errorlevel%"
popd
if not "%gameExitCode%"=="0" pause
exit /b %gameExitCode%
