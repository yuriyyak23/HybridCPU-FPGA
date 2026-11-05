$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Registers\Architectural\ArchContextState.h'
$s=[IO.File]::ReadAllText($p)
$m='namespace YAKSys_Hybrid_CPU::Core::Registers { class CommittedRegisterStorage; }'
if(-not $s.Contains($m)){throw 'Expected declaration missing'}
$s=$s.Replace($m,'namespace YAKSys_Hybrid_CPU::Core::Registers { class CommittedRegisterStorage; class CoreContextBinding; }')
$m='class StandaloneReservation(ArchContextState context) final : public Disposable'
if(-not $s.Contains($m)){throw 'Expected primary constructor missing'}
$s=$s.Replace($m,'class StandaloneReservation final : public Disposable')
$m='bool disposed = false;'
if(-not $s.Contains($m)){throw 'Expected reservation state missing'}
$s=$s.Replace($m,"std::shared_ptr<ArchContextState> context;`r`n`t`t`tbool disposed = false;")
$m='void dispose() override;'
$s=$s.Replace($m,"explicit StandaloneReservation(std::shared_ptr<ArchContextState> owner) : context(std::move(owner)) {}`r`n`t`t`tvoid dispose() override;")
$s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <utility>")
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
