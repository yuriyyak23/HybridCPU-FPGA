@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul
set "TASKROOT=C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1"
for %%F in (CompletionReasonQualificationVmReadScalarDeliveryCanonicalComposition CompletionSecondStageVmReadScalarDeliveryCanonicalComposition GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition MemoryOwnedVmReadScalarDeliveryCanonicalComposition PinBasedControlsVmReadScalarDeliveryCanonicalComposition VmReadScalarDeliveryCanonicalComposition) do (
 "C:\Program Files (x86)\Intel\oneAPI\compiler\2025.0\bin\icx-cl.exe" /nologo /c /std:c++20 /EHsc /MDd /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\ucrt" /I "%TASKROOT%" /Fo"%TASKROOT%\CompilerErrors\check-%%F.obj" "%TASKROOT%\CloseToHSL\Core\Runtime\Events\VmRead\%%F.cpp" > "%TASKROOT%\CompilerErrors\check-%%F.log" 2>&1
 call echo %%F exit=%%errorlevel%%
)
