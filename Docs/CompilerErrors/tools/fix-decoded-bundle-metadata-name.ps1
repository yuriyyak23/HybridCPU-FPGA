$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Decoder'
foreach($name in @('DecodedInstructionBundle.h','DecodedInstructionBundle.cpp')) {
 $p=Join-Path $r $name; $s=[IO.File]::ReadAllText($p)
 if(-not $s.Contains('std::shared_ptr<BundleMetadata>')){throw "Expected type not found in $name"}
 $s=$s.Replace('std::shared_ptr<BundleMetadata>','std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata>')
 [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
}
