$ErrorActionPreference='Stop'
$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps'
$files=Get-ChildItem $base -Recurse -File -Include '*.h','*.cpp' | Where-Object { $_.FullName -notmatch '\\(Legacy|obj)\\' }
foreach($f in $files){$s=[IO.File]::ReadAllText($f.FullName); if($s.Contains('Span<RetireRecord>') -or $s.Contains('shared_ptr<std::span<RetireRecord>>')){if(!(Test-Path ($f.FullName+'.before-std-span'))){Copy-Item $f.FullName ($f.FullName+'.before-std-span')}; $s=$s.Replace('std::shared_ptr<Span<RetireRecord>>','std::span<RetireRecord>').Replace('shared_ptr<std::span<RetireRecord>>','std::span<RetireRecord>').Replace('Span<RetireRecord>','std::span<RetireRecord>'); [IO.File]::WriteAllText($f.FullName,$s,[Text.UTF8Encoding]::new($false))}}
