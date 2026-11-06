@echo off
setlocal
rem Open a VS 2022 x64 / Intel oneAPI command prompt before running.
cd /d "%~dp0"
where icx-cl >nul 2>nul
if errorlevel 1 (
  echo ERROR: icx-cl not found. Open an Intel oneAPI 2025.0 x64 command prompt.
  exit /b 2
)
echo Checking Intel DPC++ compiler version:
icx-cl --version

echo.
echo STEP 1: Build FPGA emulator executable and run tests.
icx-cl -fintelfpga -DFPGA_EMULATOR=1 HybridCPU_SignExtend_HLS.cpp -o HybridCPU_SignExtend_emu.exe
if errorlevel 1 exit /b 1
HybridCPU_SignExtend_emu.exe
if errorlevel 1 exit /b 1

echo.
echo STEP 2: Generate an FPGA RTL IP core + optimization report (early link).
rem Output is a host-linkable early archive, NOT a .sv file itself.
icx-cl -fintelfpga -DFPGA_HARDWARE=1 -Xshardware -fsycl-link=early -Xstarget=Agilex7 -fsycl-device-code-split=per_kernel HybridCPU_SignExtend_HLS.cpp -o HybridCPU_SignExtend_early.lib
if errorlevel 1 exit /b 1

echo.
echo STEP 3: List generated RTL / project metadata.
if exist "HybridCPU_SignExtend_early.prj" (
  dir /s /b "HybridCPU_SignExtend_early.prj\*.sv" "HybridCPU_SignExtend_early.prj\*.v" "HybridCPU_SignExtend_early.prj\*.ip" "HybridCPU_SignExtend_early.prj\*.html"
) else (
  echo Search the current directory for a *.prj project directory.
  dir /ad /b *.prj
)
echo.
echo Done. Keep ALL generated RTL/IP dependencies, not just one .sv file.
exit /b 0
