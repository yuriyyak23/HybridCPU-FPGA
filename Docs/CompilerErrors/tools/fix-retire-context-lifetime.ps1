$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Registers\Retire'; $h=Join-Path $dir 'RetireCoordinator.h'; $c=Join-Path $dir 'RetireCoordinator.cpp'
$hs=[IO.File]::ReadAllText($h);$cs=[IO.File]::ReadAllText($c)
$old='const std::vector<std::shared_ptr<ArchContextState>>& _archContexts;'; if(-not $hs.Contains($old)){throw 'Old backing missing'}
$hs=$hs.Replace($old,'const std::shared_ptr<ArchContextCollection> _archContexts;')
$old='const std::vector<std::shared_ptr<ArchContextState>> &archContexts'; $new='std::shared_ptr<ArchContextCollection> archContexts'
if(-not $hs.Contains($old) -or -not $cs.Contains($old)){throw 'Constructor signature missing'}
$hs=$hs.Replace($old,$new);$cs=$cs.Replace($old,$new)
$old='const std::vector<std::shared_ptr<ArchContextState>> &contexts)'; $new='std::shared_ptr<ArchContextCollection> contexts)'
# HasBacking alone; application helper parameters remain vectors.
$oldSig='bool HasBacking(std::shared_ptr<PhysicalRegisterFile> registers, std::shared_ptr<RenameMap> rename, std::shared_ptr<CommitMap> commit, '+$old
$newSig='bool HasBacking(std::shared_ptr<PhysicalRegisterFile> registers, std::shared_ptr<RenameMap> rename, std::shared_ptr<CommitMap> commit, '+$new
if(-not $hs.Contains($oldSig)){throw 'HasBacking declaration missing'}; $hs=$hs.Replace($oldSig,$newSig)
$oldSig='bool RetireCoordinator::HasBacking(std::shared_ptr<PhysicalRegisterFile> registers, std::shared_ptr<RenameMap> rename, std::shared_ptr<CommitMap> commit, '+$old
$newSig='bool RetireCoordinator::HasBacking(std::shared_ptr<PhysicalRegisterFile> registers, std::shared_ptr<RenameMap> rename, std::shared_ptr<CommitMap> commit, '+$new
if(-not $cs.Contains($oldSig)){throw 'HasBacking definition missing'}; $cs=$cs.Replace($oldSig,$newSig)
$cs=$cs.Replace('VtCount(static_cast<int>(archContexts.size()))','VtCount(archContexts ? archContexts->getCount() : throw std::invalid_argument("archContexts"))').Replace('&contexts == &_archContexts','contexts == _archContexts').Replace('auto snapshot = _archContexts;','auto snapshot = _archContexts->Clone();').Replace('_archContexts.empty()','_archContexts->empty()')
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
