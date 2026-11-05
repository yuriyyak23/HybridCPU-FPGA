$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\Frontend\Invalidation\FreshnessState.h'; $s=[IO.File]::ReadAllText($p)
if($s.Contains('#include "../Contracts/RequestTypes.h"')){throw 'Include already present'}
$s=$s.Replace('#pragma once',"#pragma once`r`n`r`n#include `"../Contracts/RequestTypes.h`"")
$m='namespace HybridCpu::ShadowFrontend'; $s=$s.Replace($m,$m+"`r`n")
$m="`r`n{`r`n"; $idx=$s.IndexOf($m,$s.IndexOf('namespace HybridCpu::ShadowFrontend'))
if($idx -lt 0){throw 'Namespace opening missing'}
$s=$s.Insert($idx+$m.Length,"`tclass FreshnessSnapshot;`r`n")
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
