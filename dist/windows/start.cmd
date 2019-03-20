@echo off
cd "%~dp0"
setlocal enabledelayedexpansion

where /q /r "%programfiles%" vjoyconfig.exe 2> nul
if %errorlevel% neq 0 start /wait cmd /c .\vjoy\setup.bat

echo Server addresses:
for /f "skip=3 tokens=1,* delims=:" %%g in ('ipconfig') do (
	for /f "tokens=*" %%f in ('echo %%g ^| findstr "^[a-z]"') do (
		if not %%f  == "" (
			set iface=%%f
		)
	)
	echo %%g | findstr "IPv4" > nul && (
		< nul set /p=!iface!
		< nul set /p=http://
		< nul set /p=%%h
		< nul set /p=:31415/
		echo.
	)
)
echo.

echo Starting server...
.\websocketd\websocketd.exe --binary --port 31415 .\bin\ws_handler.exe
echo.
