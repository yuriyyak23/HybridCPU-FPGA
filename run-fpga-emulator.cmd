@echo off
setlocal
set "PATH=C:\Program Files (x86)\Intel\oneAPI\compiler\2025.0\bin;%PATH%"
set "CL_CONFIG_CPU_EMULATE_DEVICES=1"
"%~dp0x64\Debug\DPCPPConsoleApplication1.exe" %*
exit /b %errorlevel%