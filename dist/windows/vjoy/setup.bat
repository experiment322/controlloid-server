@echo off
cd "%~dp0"
setlocal enabledelayedexpansion

echo Installing vJoy driver...
.\elevate.exe -wait .\vJoySetup.exe /silent
echo.

for /f "delims=" %%v in ('where /r "%programfiles%" vjoyconfig.exe') do (
	for /l %%c in (1,1,16) do (
		echo Configuring controller %%c...
		.\elevate.exe -wait "%%v" %%c -f -a x y rx ry -b 16
	)
	echo.

	pause
	exit
)
echo.

echo Setup incomplete. Please run setup again!
pause
