$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Certificates'
$p=Join-Path $root 'YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h'
$s=[IO.File]::ReadAllText($p).Replace('#include "BundleResourceCertificate4Way.h"','')
$anchor="namespace YAKSys_Hybrid_CPU::Core`r`n{"
if (-not $s.Contains($anchor)) { $anchor="namespace YAKSys_Hybrid_CPU::Core`n{" }
if (-not $s.Contains('class BundleResourceCertificateIdentity4Way;')) { if (-not $s.Contains($anchor)) { throw 'namespace not found' }; $s=$s.Replace($anchor,"$anchor`r`n    class BundleResourceCertificateIdentity4Way;`r`n    enum class CertificateRejectDetail : unsigned char;") }
$s=$s.Replace('static inline const BundleResourceCertificate4Way Empty = YAKSys_Hybrid_CPU::Core::BundleResourceCertificate4Way();','static const BundleResourceCertificate4Way Empty;')
if (-not $s.Contains('BundleResourceCertificate4Way::Empty{}')) { $s += "`r`ninline const YAKSys_Hybrid_CPU::Core::BundleResourceCertificate4Way YAKSys_Hybrid_CPU::Core::BundleResourceCertificate4Way::Empty{};`r`n" }
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'BundleResourceCertificate4Way.h'
$s=[IO.File]::ReadAllText($p).Replace('static const unsigned short CurrentLaneTopologySignature = ComputeLaneTopologySignature();','static const unsigned short CurrentLaneTopologySignature;')
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'BundleResourceCertificate4Way.cpp'
$s=[IO.File]::ReadAllText($p)
$anchor="namespace YAKSys_Hybrid_CPU::Core`r`n{"
if (-not $s.Contains($anchor)) { $anchor="namespace YAKSys_Hybrid_CPU::Core`n{" }
if (-not $s.Contains('BundleResourceCertificateIdentity4Way::CurrentLaneTopologySignature =')) { if (-not $s.Contains($anchor)) { throw 'namespace not found' }; $s=$s.Replace($anchor,"$anchor`r`n    const unsigned short BundleResourceCertificateIdentity4Way::CurrentLaneTopologySignature = BundleResourceCertificateIdentity4Way::ComputeLaneTopologySignature();") }
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'BundleResourceCertificate.h'
$s=[IO.File]::ReadAllText($p)
$old='static inline const std::shared_ptr<BundleResourceCertificate> Empty = {CombinedMask = SafetyMask128::Zero, OwnerThreadId = -1, CycleNumber = 0, OperationCount = 0, IsSpeculative = false, BundleHash = 0, AssistStructuralKey = 0};'
$s=$s.Replace($old,'static const std::shared_ptr<BundleResourceCertificate> Empty;')
if (-not $s.Contains('BundleResourceCertificate::Empty =')) { $s += "`r`ninline const std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleResourceCertificate> YAKSys_Hybrid_CPU::Core::BundleResourceCertificate::Empty = [] {`r`n    auto certificate = std::make_shared<BundleResourceCertificate>();`r`n    certificate->CombinedMask = SafetyMask128::Zero;`r`n    certificate->OwnerThreadId = -1;`r`n    certificate->CycleNumber = 0;`r`n    certificate->OperationCount = 0;`r`n    certificate->IsSpeculative = false;`r`n    certificate->BundleHash = 0;`r`n    certificate->AssistStructuralKey = 0;`r`n    return certificate;`r`n}();`r`n" }
[IO.File]::WriteAllText($p,$s)

$p=Join-Path $root 'YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.cpp'
$s=[IO.File]::ReadAllText($p)
$include='#include "BundleResourceCertificate4Way.h"'
if (-not $s.Contains($include)) { $s=$s.Replace('#include "YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h"', "#include `"YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h`"`r`n$include") }
[IO.File]::WriteAllText($p,$s)