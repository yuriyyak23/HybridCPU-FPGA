@echo off
cd /d "C:\Users\Yuriy\Desktop\HybridCPU_FPGA"
set "TEMP=C:\Users\Yuriy\Desktop\HybridCPU_FPGA\CompilerErrors"
set "TMP=%TEMP%"
set "LIB=C:\Program Files (x86)\Intel\oneAPI\compiler\2025.0\lib;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x64;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\ucrt\x64;C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.39.33519\lib\x64;%LIB%"
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
"C:\Program Files (x86)\Intel\oneAPI\compiler\2025.0\bin\icx-cl.exe" @CompilerErrors\independent-incremental-link.rsp >CompilerErrors\independent-incremental-link.log 2>&1
set "LINK_RESULT=%ERRORLEVEL%"
>CompilerErrors\independent-incremental-link.exit.txt echo %LINK_RESULT%
exit /b %LINK_RESULT%