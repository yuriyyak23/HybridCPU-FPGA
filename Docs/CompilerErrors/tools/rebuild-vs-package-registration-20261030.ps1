$ErrorActionPreference = 'Stop'
if (Get-Process devenv,MSBuild -ErrorAction SilentlyContinue) { throw 'Close Visual Studio and MSBuild first.' }
$devenv = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.com'
$log = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA\CompilerErrors\devenv-setup-after-vsix-elevated-20261030.log'
$summary = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA\CompilerErrors\devenv-setup-after-vsix-summary-20261030.log'
try {
  & $devenv /setup 2>&1 | Tee-Object -FilePath $log
  $code = $LASTEXITCODE
  "devenv /setup exit code: $code" | Set-Content -LiteralPath $summary -Encoding UTF8
  if ($code -ne 0) { throw "devenv /setup failed: exit $code" }
} catch {
  ("Failure: " + $_.Exception.Message) | Set-Content -LiteralPath $summary -Encoding UTF8
  throw
}
