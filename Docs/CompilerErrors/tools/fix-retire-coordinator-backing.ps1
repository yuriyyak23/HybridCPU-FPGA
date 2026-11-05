$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Registers\Retire'; $h=Join-Path $dir 'RetireCoordinator.h'; $c=Join-Path $dir 'RetireCoordinator.cpp'
$hs=[IO.File]::ReadAllText($h); $cs=[IO.File]::ReadAllText($c)
$old='const std::vector<std::shared_ptr<ArchContextState>> _archContexts;'; $new='const std::vector<std::shared_ptr<ArchContextState>>& _archContexts;'
if(-not $hs.Contains($old)){throw 'Contexts field missing'}; $hs=$hs.Replace($old,$new)
$old=': _physicalRegisters((physicalRegisters != nullptr)'; $new=': VtCount(static_cast<int>(archContexts.size())), _physicalRegisters((physicalRegisters != nullptr)'
if(-not $cs.Contains($old)){throw 'Initializer missing'}; $cs=$cs.Replace($old,$new)
$cs=$cs.Replace('_archContexts(archContexts ? archContexts : throw std::invalid_argument("archContexts"))','_archContexts(archContexts)')
$cs=$cs.Replace("`r`n`t`tVtCount = _archContexts.size();",'')
$cs=$cs.Replace('this->ReferenceEquals(contexts, _archContexts)','&contexts == &_archContexts')
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
