$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\Frontend\Contracts'
$p=Join-Path $r 'Contracts.h'; $s=[IO.File]::ReadAllText($p); $m='ImmutableArray<std::shared_ptr<SemanticSlot>>'
if(-not $s.Contains($m)){throw 'Expected immutable array declarations missing'}
$s=$s.Replace($m,'std::vector<std::shared_ptr<SemanticSlot>>')
$s=$s.Replace('namespace HybridCpu::ShadowFrontend' + "`r`n" + '{', 'namespace HybridCpu::ShadowFrontend' + "`r`n" + '{' + "`r`n" + '    class AccessDecision; class CanonicalFetchResult;')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Contracts.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace($m,'std::vector<std::shared_ptr<SemanticSlot>>')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
