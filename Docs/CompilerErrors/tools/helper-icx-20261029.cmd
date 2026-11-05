@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul
"C:\Program Files (x86)\Intel\oneAPI\compiler\2025.0\bin\icx-cl.exe" /fsycl /c /ZI /W3 /Od /fintelfpga /nologo /D FPGA_EMULATOR=1 /D _DEBUG /D _UNICODE /D UNICODE /WX- /EHsc /std:c++20 /MDd /I"C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\ucrt" /Fo"C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\CompilerErrors\helper-obj\ReadRangeMetadataHelper.obj" /Zc:__cplusplus "C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Metadata\ReadRangeMetadataHelper.cpp" > "C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\CompilerErrors\helper-icx-20261029.log" 2>&1
exit /b %errorlevel%

