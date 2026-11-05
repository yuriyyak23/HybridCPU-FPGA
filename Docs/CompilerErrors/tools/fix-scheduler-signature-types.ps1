$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$dir=Join-Path $root 'CloseToHSL\\Core\Pipeline\Assist\InterCore'
foreach($file in @('YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h','YAKSys_Hybrid_CPU.Core.MicroOpScheduler.cpp')){
 $p=Join-Path $dir $file;$s=Get-Content $p -Raw
 $s=$s.Replace('SafetyVerifier::VirtualizationAdmissionCertificate','VirtualizationAdmissionCertificate').Replace('unsigned char::MaxValue','std::numeric_limits<unsigned char>::max()').Replace('unsigned short::MaxValue','std::numeric_limits<unsigned short>::max()')
 if($file.EndsWith('.h')){$s=$s.Replace('struct ResearchVirtualizationCanonicalCompositionResult;','struct ResearchVirtualizationCanonicalCompositionResult; class ResearchVirtualizationCanonicalIssueComposition;')}
 Set-Content $p $s -Encoding UTF8 -NoNewline
}
