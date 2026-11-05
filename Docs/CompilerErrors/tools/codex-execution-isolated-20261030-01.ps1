$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$log = Join-Path $root 'CompilerErrors\codex-execution-isolated-20261030-01.log'
$intDir = Join-Path $root 'CompilerErrors\codex-execution-isolated-obj\'
$project = Get-Content (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') -Raw
$sources = [regex]::Matches($project, '<ClCompile Include="(2\\CloseToHSL\\Core\\Execution\\ExternalAccelerators\\(?:Auth|Backends)\\[^"]+\.cpp)"') | ForEach-Object { $_.Groups[1].Value }
foreach ($source in $sources) {
    Add-Content -LiteralPath $log -Value "`nSOURCE=$source"
    & $msbuild (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$source" "/p:IntDir=$intDir" /m:1 /v:minimal /nologo *>> $log
    Add-Content -LiteralPath $log -Value "MSBUILD_EXIT=$LASTEXITCODE"
}