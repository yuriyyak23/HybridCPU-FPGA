$path=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Memory\Timing\MemoryCycleController.cpp'
$line=Get-Content $path | Where-Object { $_ -match '^\s*if \(\(?size\s+(is not|!=)' } | Select-Object -First 1
if(-not $line) {throw 'Size condition not found'}
$code='bool invalid(int size) {' + $line + ' return true; return false; }' + "`r`n" + 'int main() { return !(invalid(0) && !invalid(1) && !invalid(2) && invalid(3) && !invalid(4) && !invalid(8) && invalid(9)); }'
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'negative-pattern-check.cpp'),$code,[Text.Encoding]::ASCII)