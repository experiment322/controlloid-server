@echo off
cd "%~dp0"
setlocal enabledelayedexpansion

echo Installing vJoy driver...
start /wait .\vJoySetup.exe /silent
echo.

if "%programw6432%"=="" (
	set programs=%programfiles%
) else (
	set programs=%programw6432%
)

for /f "delims=" %%v in ('where /r "!programs!" vjoyconfig.exe') do (
	echo Resetting vJoy...
	"%%v" -r

	for /l %%c in (1,1,4) do (
		echo Configuring controller %%c...
		"%%v" %%c -f -a x y rx ry -b 16
	)
	echo.

	pause
	exit
)
echo.

echo Setup incomplete. Please run setup again!
pause
