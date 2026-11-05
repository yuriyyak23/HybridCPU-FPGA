$ErrorActionPreference = 'Stop'
$vsRoot = 'C:\Program Files\Microsoft Visual Studio\2022\Professional'
$vsixInstaller = Join-Path $vsRoot 'Common7\IDE\VSIXInstaller.exe'
$vsix = 'C:\Program Files (x86)\Intel\oneAPI\dpcpp-ct\2025.0\share\ide_support\visual_studio\dpct\vs2022\Intel.DPCPPCT.vsix'
$extensionId = 'B247EB12-3C2F-40ED-8B98-B3F547BAD136'
$logDir = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA\CompilerErrors'
$uninstallLog = Join-Path $logDir 'intel-dpct-vsix-uninstall-retry-20261030.log'
$installLog = Join-Path $logDir 'intel-dpct-vsix-install-retry-20261030.log'
$summary = Join-Path $logDir 'intel-dpct-vsix-repair-summary-retry-20261030.log'
$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = [Security.Principal.WindowsPrincipal]::new($identity)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) { throw 'Run elevated; the extension is installed for all users.' }
if ((Get-Item $vsix).Length -eq 0) { throw "Source VSIX is empty: $vsix" }
if (-not (Test-Path $vsixInstaller)) { throw "VSIXInstaller not found: $vsixInstaller" }
try {
  $installedManifest = Join-Path $vsRoot 'Common7\IDE\Extensions\1fa51aoj.24x\extension.vsixmanifest'
  if (Test-Path $installedManifest) {
    $uninstall = Start-Process -FilePath $vsixInstaller -ArgumentList "/quiet /uninstall:$extensionId /logFile:`"$uninstallLog`"" -Wait -PassThru
    if ($uninstall.ExitCode -ne 0) { throw "VSIX uninstall failed with exit code $($uninstall.ExitCode)." }
    "Uninstall exit: $($uninstall.ExitCode)" | Set-Content -LiteralPath $summary -Encoding UTF8
  } else {
    'Uninstall skipped: target VSIX is already removed.' | Set-Content -LiteralPath $summary -Encoding UTF8
  }
  $installArgs = "/quiet /admin /instanceIds:3740fc72 `"$vsix`" /logFile:`"$installLog`""
  $install = Start-Process -FilePath $vsixInstaller -ArgumentList $installArgs -Wait -PassThru
  if ($install.ExitCode -ne 0) { throw "VSIX install failed with exit code $($install.ExitCode)." }
  "Install exit: $($install.ExitCode)" | Add-Content -LiteralPath $summary -Encoding UTF8
  'Result: success' | Add-Content -LiteralPath $summary -Encoding UTF8
} catch {
  ("Result: failure`r`n" + $_.Exception.Message) | Add-Content -LiteralPath $summary -Encoding UTF8
  throw
}


