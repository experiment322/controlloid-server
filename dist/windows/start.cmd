@echo off

echo Server addresses:
for /f "skip=3 tokens=1,* delims=:" %%g in ('ipconfig') do (
	for /f "tokens=*" %%f in ('echo %%g ^| findstr "^[a-z]"') do (
		if not %%f  == "" (
			echo | set /p=%%f
		)
	)
	echo %%g | findstr "IPv4" > nul && (
		echo | set /p=http://
		echo | set /p=%%h
		echo | set /p=:31415/
		echo(
	)
)
echo(

echo Starting server...
.\websocketd\websocketd.exe --binary --port 31415 .\bin\ws_handler.exe

