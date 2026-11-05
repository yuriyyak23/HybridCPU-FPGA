$ErrorActionPreference = 'Stop'
$vsRoot = 'C:\Program Files\Microsoft Visual Studio\2022\Professional'
$installer = Join-Path $vsRoot 'Common7\IDE\VSIXInstaller.exe'
$extensionId = 'B247EB12-3C2F-40ED-8B98-B3F547BAD136'
$manifest = Join-Path $vsRoot 'Common7\IDE\Extensions\1fa51aoj.24x\extension.vsixmanifest'
$summary = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA\CompilerErrors\remove-unsupported-intel-dpct-vsix-summary-20261030.log'
$log = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA\CompilerErrors\remove-unsupported-intel-dpct-vsix-20261030.log'
if (Get-Process devenv,MSBuild,vshost -ErrorAction SilentlyContinue) { throw 'Close Visual Studio, MSBuild, and vshost before proceeding.' }
$identity=[Security.Principal.WindowsIdentity]::GetCurrent()
$principal=[Security.Principal.WindowsPrincipal]::new($identity)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) { throw 'Run elevated; this VSIX is installed for all users.' }
if (-not (Test-Path $manifest)) { throw 'Expected installed DPCT VSIX manifest not found.' }
$p=Start-Process -FilePath $installer -ArgumentList "/quiet /uninstall:$extensionId /logFile:`"$log`"" -Wait -PassThru
"Uninstall exit code: $($p.ExitCode)" | Set-Content -LiteralPath $summary -Encoding UTF8
if ($p.ExitCode -ne 0) { throw "VSIX uninstall failed with exit code $($p.ExitCode)." }
