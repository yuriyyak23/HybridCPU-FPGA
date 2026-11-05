$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$stem = [IO.Path]::ChangeExtension($PSCommandPath, $null)
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.sln') /m /t:Build /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo /fl "/flp:logfile=$stem.log;verbosity=normal" *> "$stem.console.log"
$code = $LASTEXITCODE
Add-Content -Path "$stem.console.log" -Value "MSBUILD_EXIT=$code"
exit $code
