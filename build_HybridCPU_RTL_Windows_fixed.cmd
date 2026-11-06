@echo off
setlocal EnableExtensions

rem Intel oneAPI 2025.0 FPGA HLS - Windows x64.
rem Run from an Intel oneAPI + Visual Studio x64 command prompt.
rem Keep this script and HybridCPU_SignExtend_HLS.cpp in the same folder.

set "ROOT=%~dp0"
set "SOURCE=%ROOT%HybridCPU_SignExtend_HLS.cpp"
set "EMU=%ROOT%HybridCPU_SignExtend_emu.exe"
set "EARLY=%ROOT%HybridCPU_SignExtend_early.obj"
set "PROJECT=%ROOT%HybridCPU_SignExtend_early.prj"

pushd "%ROOT%" || (
  echo ERROR: Cannot enter script directory "%ROOT%".
  exit /b 2
)

echo Source: "%SOURCE%"
if not exist "%SOURCE%" (
  echo ERROR: Missing HybridCPU_SignExtend_HLS.cpp.
  echo Copy HybridCPU_SignExtend_HLS.cpp next to this CMD script.
  popd
  exit /b 2
)

where icx-cl >nul 2>nul
if errorlevel 1 (
  echo ERROR: icx-cl not found. Open an Intel oneAPI 2025.0 x64 prompt.
  popd
  exit /b 2
)

echo.
echo Compiler version:
icx-cl --version
if errorlevel 1 goto :fail

echo.
echo STEP 1: Build FPGA emulator and run 8 tests.
rem /EHsc is REQUIRED here because host main uses try/catch.
rem /Od /DEBUG are emulator debug options, NOT hardware HLS options.
icx-cl -fsycl -fintelfpga /EHsc /Od /DEBUG -DFPGA_EMULATOR=1 "%SOURCE%" -o "%EMU%"
if errorlevel 1 goto :fail

"%EMU%"
if errorlevel 1 goto :fail

echo.
echo STEP 2: Generate FPGA HLS RTL IP and optimization report.
rem /O2 instead of /Od: leave optimizations ON for hardware HLS.
rem FPGA_HARDWARE selects the correct device in the host code;
rem the early image is generated, not executed on an FPGA board.
icx-cl -fsycl -fintelfpga /EHsc /O2 -DFPGA_HARDWARE=1 -Xshardware -fsycl-link=early -Xstarget=Agilex7 -fsycl-device-code-split=per_kernel "%SOURCE%" -o "%EARLY%"
if errorlevel 1 goto :fail

echo.
echo STEP 3: Locate generated HDL, IP, and reports.
if exist "%PROJECT%\" (
  echo Project folder: "%PROJECT%"
  dir /s /b "%PROJECT%\*.sv" 2>nul
  dir /s /b "%PROJECT%\*.v" 2>nul
  dir /s /b "%PROJECT%\*.ip" 2>nul
  dir /s /b "%PROJECT%\*.html" 2>nul
) else (
  echo NOTE: Expected .prj directory not found at "%PROJECT%".
  echo Searching for *.prj directories in this folder:
  dir /ad /b "*.prj" 2>nul
)

echo.
echo SUCCESS: Emulator tests passed and early HLS compile finished.
echo Keep the whole generated *.prj directory for RTL dependencies.
popd
exit /b 0

:fail
echo.
echo ERROR: Build or emulator execution failed; inspect the first error above.
popd
exit /b 1
